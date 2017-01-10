#include "graphics.h"
#include "options.h"
#include "helper_functions.h"

extern options g_options;

static UINT s_descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

UINT graphics::descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	return s_descriptor_sizes[type];
}

void graphics::create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask)
{
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc{ type, num_descriptors, flags, node_mask };
	ThrowIfFailed(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_descriptor_heaps[type].GetAddressOf())));

	m_current_gpu_handle[type] = m_descriptor_heaps[type]->GetGPUDescriptorHandleForHeapStart();
	m_current_cpu_handle[type] = m_descriptor_heaps[type]->GetCPUDescriptorHandleForHeapStart();
}


void graphics::increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE const type)
{
	m_current_gpu_handle[type].ptr += graphics::descriptor_size(type);
	m_current_cpu_handle[type].ptr += graphics::descriptor_size(type);
}

bool graphics::initialize(HWND main_window_handle)
{
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ComPtr<IDXGIFactory4> dxgi_factory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory)));
	
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d_device))))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice( pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d_device)));
	}

	for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		s_descriptor_sizes[i] = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(i));

	ComPtr<ID3D12CommandAllocator> initialization_command_allocator;
	ComPtr<ID3D12GraphicsCommandList> initialization_command_list;
	ThrowIfFailed(m_d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(initialization_command_allocator.GetAddressOf())));
	ThrowIfFailed(m_d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, initialization_command_allocator.Get(), nullptr, IID_PPV_ARGS(initialization_command_list.GetAddressOf())));

	ThrowIfFailed(m_d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	D3D12_COMMAND_QUEUE_DESC queueDesc{ D3D12_COMMAND_LIST_TYPE_DIRECT, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
	ThrowIfFailed(m_d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_command_queue)));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{ (UINT)g_options.screen_width, (UINT)g_options.screen_height, DXGI_FORMAT_R8G8B8A8_UNORM, false, { 1, 0 },
		DXGI_USAGE_RENDER_TARGET_OUTPUT, frames_count, DXGI_SCALING_STRETCH, DXGI_SWAP_EFFECT_FLIP_DISCARD, DXGI_ALPHA_MODE_UNSPECIFIED, 0 };	

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(dxgi_factory->CreateSwapChainForHwnd( m_command_queue.Get(), main_window_handle, &swapChainDesc, nullptr, nullptr, &swapChain ));
	ThrowIfFailed(dxgi_factory->MakeWindowAssociation( main_window_handle, DXGI_MWA_NO_ALT_ENTER ));
	ThrowIfFailed(swapChain.As(&m_swap_chain));
	m_current_frame_index = m_swap_chain->GetCurrentBackBufferIndex();
	
	create_descriptor_heap( m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, frames_count*(render_targets_count+1), D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 );
	create_descriptor_heap( m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, frames_count*2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 );
	create_descriptor_heap( m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, _countof(Textures) + frames_count*(render_targets_count+5), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 );
	create_descriptor_heap( m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 );
	
	for (UINT i = 0; i < frames_count; i++)
	{
		ThrowIfFailed(m_d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_direct_command_list_allocators[i].GetAddressOf())));
		ThrowIfFailed(m_d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_direct_command_list_allocators[i].Get(), nullptr, IID_PPV_ARGS(m_command_lists[i].GetAddressOf())));
		ThrowIfFailed(m_command_lists[i]->Close());
	}

	for (UINT i = 0; i < frames_count; i++)
	{
		m_swap_chain_buffers[i].initialize_rtv(m_d3d_device.Get(), this, m_swap_chain.Get(), i);

		m_start_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[i].resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_end_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[i].resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(m_d3d_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;	

	D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
		{D3D12_DESCRIPTOR_RANGE_TYPE_SRV,		2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },	// 2 frequently changed diffuse + normal textures - using registers t1 and t2.
		{D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },	// 1 frequently changed constant buffer.
		{D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,	1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_NONE,								D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
	};
	
	CD3DX12_ROOT_PARAMETER1 root_parameters[3];
	root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	root_parameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(m_d3d_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));
	
	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> pixel_shader;

	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr));

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{ StandardVertexDescription, _countof(StandardVertexDescription) };

	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{ true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL, FALSE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
												{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS },
												{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS } 
	};
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.InputLayout = inputLayoutDesc;
	pso_desc.pRootSignature = m_root_signature.Get();
	pso_desc.VS = { vertex_shader->GetBufferPointer(), vertex_shader->GetBufferSize() };
	pso_desc.PS = { pixel_shader->GetBufferPointer(), pixel_shader->GetBufferSize() };
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState = depth_stencil_desc;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.NumRenderTargets = render_targets_count + 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_SNORM;
	pso_desc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_SNORM;
	pso_desc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_SNORM;
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pso_desc.SampleDesc.Count = 1;
	ThrowIfFailed(m_d3d_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_state)));
	
	for (int i = 0; i < frames_count; ++i)
	{
		m_depth_stencils[i].initialize(m_d3d_device.Get(), 0, g_options.screen_width, g_options.screen_height, 1, DXGI_FORMAT_R32_TYPELESS, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN,
			&clear_value(DXGI_FORMAT_D32_FLOAT, D3D12_DEPTH_STENCIL_VALUE{ 1.0f,0 }), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, L"depth_stencil", i);
		m_depth_stencils[i].initialize_dsv(m_d3d_device.Get(), DXGI_FORMAT_D32_FLOAT, this);
		m_depth_stencils[i].initialize_cbv_srv_uav(m_d3d_device.Get(), DXGI_FORMAT_R32_FLOAT, this);
	}
	
	m_screen_viewport.TopLeftX = 0;
	m_screen_viewport.TopLeftY = 0;
	m_screen_viewport.Width = static_cast<float>(g_options.screen_width);
	m_screen_viewport.Height = static_cast<float>(g_options.screen_height);
	m_screen_viewport.MinDepth = 0.0f;
	m_screen_viewport.MaxDepth = 1.0f;
	m_scissor_rectangle = { 0, 0, g_options.screen_width, g_options.screen_height };
	
	UINT fileSize = 0;
	UINT8* pAssetData;
	ThrowIfFailed(ReadDataFromFile(DataFileName, &pAssetData, &fileSize));

	m_vertex_buffer.initialize(m_d3d_device.Get(), initialization_command_list.Get(), pAssetData + VertexDataOffset, VertexDataSize, StandardVertexStride);
	m_index_buffer.initialize(m_d3d_device.Get(), initialization_command_list.Get(), pAssetData + IndexDataOffset, IndexDataSize, StandardIndexFormat);
		
	for (UINT i = 0, n = _countof(Textures); i < n; i++)
	{
		TextureResource const& tex = Textures[i];
		m_textures[i].initialize( m_d3d_device.Get(), 0, tex.Width, tex.Height, tex.MipLevels, tex.Format, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, nullptr, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_FLAG_NONE, L"texture", i);
		m_textures[i].initialize_with_data(m_d3d_device.Get(), initialization_command_list.Get(), tex.Data->Pitch, tex.Data->Size, pAssetData + tex.Data->Offset);
		m_textures[i].initialize_cbv_srv_uav(m_d3d_device.Get(), tex.Format, this);
	}

	free(pAssetData);
	
	m_wrap_sampler.initialize(m_d3d_device.Get(), this, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	
	for (int i = 0; i < NumLights; i++)
	{
		m_lights[i].position = { 0.0f, 15.0f, -30.0f, 1.0f };
		m_lights[i].direction = { 0.0, 0.0f, 1.0f, 0.0f };
		m_lights[i].falloff = { 800.0f, 1.0f, 0.0f, 1.0f };
		m_lights[i].color = { 0.7f, 0.7f, 0.7f, 1.0f };
	}

	UINT const textureCount = _countof(Textures);

	for (auto& buffer : m_transform_constants_buffers)
	{
		buffer.initialize(m_d3d_device.Get(), 1);
		buffer.initialize_cbv(m_d3d_device.Get(), this);
	}

	for (auto& buffer : m_g_buffer_constants)
	{
		buffer.initialize(m_d3d_device.Get(), 1);
		buffer.initialize_cbv(m_d3d_device.Get(), this);
	}

	m_g_buffer.initialize(m_d3d_device.Get(), initialization_command_list.Get(), this, &pso_desc.RTVFormats[1]);

	m_shadow_map.initialize(m_d3d_device.Get(), this);
	
	ThrowIfFailed(initialization_command_list->Close());

	ID3D12CommandList* cmdsLists[] = { initialization_command_list.Get() };
	m_command_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	
	ThrowIfFailed(m_command_queue->Signal(m_fence.Get(), 1));

	m_fence_event = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

	if (m_fence->GetCompletedValue() < 1)
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(1, m_fence_event));
		WaitForSingleObject(m_fence_event, INFINITE);
	}
	
	update(0);

	return true;
}