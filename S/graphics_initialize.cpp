#include "graphics.h"
#include "options.h"
#include "helper_functions.h"
#include "d3dx12.h"

extern options g_options;

UINT graphics::descriptor_size(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	return m_descriptor_sizes[type];
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

enum {
	upload_buffer_size = 1024 * 1024,
};
static ComPtr<ID3D12Resource> s_upload_buffer_resource;
static char* s_upload_buffer_data_begin		= nullptr;
static char* s_upload_buffer_data_current	= nullptr;
static char* s_upload_buffer_data_end		= nullptr;

constant_buffer_data graphics::create_constant_buffer_view(unsigned const buffer_size)
{
	auto const aligned_size = aligned(buffer_size, 256);
	assert(aligned_size <= 65536);
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{
		s_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(s_upload_buffer_data_current - s_upload_buffer_data_begin),
		aligned_size
	};
	m_d3d_device->CreateConstantBufferView(&cbv_desc, current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	
	constant_buffer_data result{ 
		static_cast<void*>(s_upload_buffer_data_current), 
		current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) 
	};

	s_upload_buffer_data_current += aligned_size;
	assert(s_upload_buffer_data_current < s_upload_buffer_data_end);
	increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return result;
}

void graphics::initialize_constant_buffers()
{
	m_d3d_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(s_upload_buffer_resource.GetAddressOf())
	);

	void* data;
	D3D12_RANGE read_range{ 0, upload_buffer_size };
	s_upload_buffer_resource->Map(0, &read_range, &data);
	s_upload_buffer_data_current = s_upload_buffer_data_begin = static_cast<char*>(data);
	s_upload_buffer_data_end = s_upload_buffer_data_begin + upload_buffer_size;
	
	for (auto& data : m_per_frame_constants)
		data = create_constant_buffer_view(sizeof(per_frame_constants));
}

ID3D12PipelineState*		graphics::pipeline_state(unsigned const id)
{
	if (m_pipeline_states[id].Get())
		return m_pipeline_states[id].Get();

	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> pixel_shader;
	ComPtr<ID3DBlob> errors;

	auto const render_objects_count_string = std::to_string(render_objects_count);

	D3D_SHADER_MACRO const shader_macros[] = {
		{ "RENDER_OBJECTS_COUNT", render_objects_count_string.c_str() },
		{ nullptr, nullptr }
	};

	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//xy_position_x_model_view_projection.hlsl", shader_macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1", compile_flags, 0, &vertex_shader, &errors), errors);
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//color_id.hlsl", shader_macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1", compile_flags, 0, &pixel_shader, &errors), errors);

	D3D12_INPUT_ELEMENT_DESC input_element_description[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC input_layout_description{ input_element_description, _countof(input_element_description) };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.InputLayout = input_layout_description;
	pso_desc.pRootSignature = root_signature(root_signatures::one);
	pso_desc.VS = { vertex_shader->GetBufferPointer(), vertex_shader->GetBufferSize() };
	pso_desc.PS = { pixel_shader->GetBufferPointer(), pixel_shader->GetBufferSize() };
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState.DepthEnable = FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.NumRenderTargets = 2;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.RTVFormats[1] = DXGI_FORMAT_R32_UINT;
	pso_desc.SampleDesc.Count = 1;
	
	switch (id)
	{
	case pipeline_states::triangle_one:
	{
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		ThrowIfFailed(m_d3d_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_states[id])));
		break;
	}
	case pipeline_states::line_one:
	{
		pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		ThrowIfFailed(m_d3d_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_states[id])));
		break;
	}
	default:
		assert(true);
	}

	return m_pipeline_states[id].Get();
}
ID3D12RootSignature*		graphics::root_signature(unsigned const id)
{
	if (m_root_signatures[id].Get())
		return m_root_signatures[id].Get();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (static bool first_time = true)
	{
		if (FAILED(m_d3d_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		first_time = false;
	}

	switch (id)
	{
	case root_signatures::one:
	{
		D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
			{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
		};

		CD3DX12_ROOT_PARAMETER1 root_parameters[2];
		root_parameters[0].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
		root_parameters[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
		root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error), error);
		ThrowIfFailed(m_d3d_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signatures[id])));
		break;
	}
	default:
		assert(true);
	}

	return m_root_signatures[id].Get();
}
D3D12_VERTEX_BUFFER_VIEW*	graphics::vertex_buffer_view(void const* const vertices, unsigned const vertices_size, unsigned const vertex_size, unsigned const index)
{
	for (unsigned i = 0; i < m_vertex_buffer_views_count; ++i)
	{
		if (index == m_vertex_buffer_views[i].second)
			return &m_vertex_buffer_views[i].first;
	}

	memcpy(s_upload_buffer_data_current, vertices, vertices_size);
	D3D12_VERTEX_BUFFER_VIEW const view = {
		s_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(s_upload_buffer_data_current - s_upload_buffer_data_begin),
		vertices_size,
		vertex_size
	};

	m_vertex_buffer_views[m_vertex_buffer_views_count++] = { view, index };
	assert(m_vertex_buffer_views_count < max_vertex_buffer_views_count);

	s_upload_buffer_data_current += aligned(vertices_size, 4);
	assert(s_upload_buffer_data_current < s_upload_buffer_data_end);

	return &m_vertex_buffer_views[m_vertex_buffer_views_count - 1].first;
}

D3D12_INDEX_BUFFER_VIEW*	graphics::index_buffer_view(void const* const indices, unsigned const indices_size, DXGI_FORMAT const format, unsigned const index)
{
	for (unsigned i = 0; i < m_index_buffer_views_count; ++i)
	{
		if (index == m_index_buffer_views[i].second)
			return &m_index_buffer_views[i].first;
	}

	memcpy(s_upload_buffer_data_current, indices, indices_size);
	D3D12_INDEX_BUFFER_VIEW const view{
		s_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(s_upload_buffer_data_current - s_upload_buffer_data_begin),
		indices_size,
		format
	};

	m_index_buffer_views[m_index_buffer_views_count++] = { view, index };
	assert(m_index_buffer_views_count < max_index_buffer_views_count);

	s_upload_buffer_data_current += aligned(indices_size, 4);
	assert(s_upload_buffer_data_current < s_upload_buffer_data_end);

	return &m_index_buffer_views[m_index_buffer_views_count - 1].first;
}

bool graphics::initialize(HWND main_window_handle)
{
	increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, _countof(m_per_frame_constants));
	
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
		m_descriptor_sizes[i] = m_d3d_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(i));

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
		
	if (m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] > 0)
		create_descriptor_heap( m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV], D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 );
	if (m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] > 0)
		create_descriptor_heap(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER], D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0);
	if (m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] > 0)
		create_descriptor_heap(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_RTV], D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0);
	if (m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] > 0)
		create_descriptor_heap(m_d3d_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_DSV], D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0);
	
	for (UINT i = 0; i < frames_count; i++)
	{
		ThrowIfFailed(m_d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_direct_command_list_allocators[i].GetAddressOf())));
		ThrowIfFailed(m_d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_direct_command_list_allocators[i].Get(), nullptr, IID_PPV_ARGS(m_command_lists[i].GetAddressOf())));
		ThrowIfFailed(m_command_lists[i]->Close());
	}

	for (UINT i = 0; i < frames_count; i++)
		m_swap_chain_buffers[i].initialize_rtv(m_d3d_device.Get(), m_swap_chain.Get(), i);

	for (unsigned i = 0; i < frames_count; i++)
	{
		D3D12_CLEAR_VALUE clear_value;
		clear_value.Format = DXGI_FORMAT_R32_UINT;
		std::fill(std::begin(clear_value.Color), std::end(clear_value.Color), 0.0f);

		m_indices_render_targets[i].initialize(m_d3d_device.Get(), 0, g_options.screen_width, g_options.screen_height, 1, DXGI_FORMAT_R32_UINT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, &clear_value, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, L"render_target", i);
		m_indices_render_targets[i].initialize_rtv(m_d3d_device.Get(), DXGI_FORMAT_R32_UINT);
		m_indices_render_targets[i].initialize_readback_resource(m_d3d_device.Get());
	}
		
	m_screen_viewport.TopLeftX = 0;
	m_screen_viewport.TopLeftY = 0;
	m_screen_viewport.Width = static_cast<float>(g_options.screen_width);
	m_screen_viewport.Height = static_cast<float>(g_options.screen_height);
	m_screen_viewport.MinDepth = 0.0f;
	m_screen_viewport.MaxDepth = 1.0f;
	m_scissor_rectangle = { 0, 0, g_options.screen_width, g_options.screen_height };

	initialize_constant_buffers();

	m_square_field.initialize( this );
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

	return true;
}