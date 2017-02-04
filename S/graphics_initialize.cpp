#include "graphics.h"
#include "options.h"
#include "helper_functions.h"
#include "d3dx12.h"

extern options g_options;

unsigned g_current_frame_index = 0;

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

constant_buffer_data graphics::create_constant_buffer_view(unsigned const buffer_size)
{
	auto const aligned_size = aligned(buffer_size, 256);
	assert(aligned_size <= 65536);
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{
		m_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(m_upload_buffer_data_current - m_upload_buffer_data_begin),
		aligned_size
	};
	m_d3d_device->CreateConstantBufferView(&cbv_desc, current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	
	constant_buffer_data result{ 
		static_cast<void*>(m_upload_buffer_data_current), 
		current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) 
	};

	m_upload_buffer_data_current += aligned_size;
	assert(m_upload_buffer_data_current < m_upload_buffer_data_end);
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
		IID_PPV_ARGS(m_upload_buffer_resource.GetAddressOf())
	);

	void* data;
	D3D12_RANGE read_range{ 0, upload_buffer_size };
	m_upload_buffer_resource->Map(0, &read_range, &data);
	m_upload_buffer_data_current = m_upload_buffer_data_begin = static_cast<char*>(data);
	m_upload_buffer_data_end = m_upload_buffer_data_begin + upload_buffer_size;

	for (unsigned i = 0; i < frames_count; ++i)
	{
		m_per_frame_model_view_projections[i] = create_constant_buffer_view(sizeof(model_view_projection_constants));
		m_per_frame_colors[i] = create_constant_buffer_view(sizeof(model_view_projection_constants));
	}
}

ID3D12PipelineState* graphics::pipeline_state(
	ID3D12RootSignature* const root_signature,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE const primitive_topology_type,
	D3D12_INPUT_LAYOUT_DESC const& input_layout_description,
	ID3DBlob* const vertex_shader,
	ID3DBlob* const pixel_shader
)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.pRootSignature = root_signature;
	pso_desc.VS = { vertex_shader->GetBufferPointer(), vertex_shader->GetBufferSize() };
	pso_desc.PS = { pixel_shader->GetBufferPointer(), pixel_shader->GetBufferSize() };
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState.DepthEnable = FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.NumRenderTargets = 2;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.RTVFormats[1] = indices_render_target_format;
	pso_desc.SampleDesc.Count = 1;
	pso_desc.InputLayout = input_layout_description;
	pso_desc.PrimitiveTopologyType = primitive_topology_type;
	ThrowIfFailed(m_d3d_device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_states[m_pipeline_states_count])));
	m_pipeline_states_count++;

	return m_pipeline_states[m_pipeline_states_count-1].Get();
}
ID3D12RootSignature*		graphics::root_signature()
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (static bool first_time = true)
	{
		if (FAILED(m_d3d_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		first_time = false;
	}

	D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
		{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		2, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CD3DX12_ROOT_PARAMETER1 root_parameters[2];
	root_parameters[0].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
	root_parameters[1].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error), error);
	ThrowIfFailed(m_d3d_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signatures[m_root_signatures_count])));
	m_root_signatures_count++;

	return m_root_signatures[m_root_signatures_count-1].Get();
}
D3D12_VERTEX_BUFFER_VIEW*	graphics::vertex_buffer_view(void const* const vertices, unsigned const vertices_size, unsigned const vertex_size)
{
	memcpy(m_upload_buffer_data_current, vertices, vertices_size);
	m_vertex_buffer_views[m_vertex_buffer_views_count++] = {
		m_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(m_upload_buffer_data_current - m_upload_buffer_data_begin),
		vertices_size,
		vertex_size
	};
	assert(m_vertex_buffer_views_count < max_vertex_buffer_views_count);

	m_upload_buffer_data_current += aligned(vertices_size, 4);
	assert(m_upload_buffer_data_current < m_upload_buffer_data_end);

	return &m_vertex_buffer_views[m_vertex_buffer_views_count - 1];
}

D3D12_INDEX_BUFFER_VIEW*	graphics::index_buffer_view(void const* const indices, unsigned const indices_size, DXGI_FORMAT const format)
{
	memcpy(m_upload_buffer_data_current, indices, indices_size);
	m_index_buffer_views[m_index_buffer_views_count++] = {
		m_upload_buffer_resource->GetGPUVirtualAddress() + static_cast<unsigned>(m_upload_buffer_data_current - m_upload_buffer_data_begin),
		indices_size,
		format
	};
	assert(m_index_buffer_views_count < max_index_buffer_views_count);

	m_upload_buffer_data_current += aligned(indices_size, 4);
	assert(m_upload_buffer_data_current < m_upload_buffer_data_end);

	return &m_index_buffer_views[m_index_buffer_views_count - 1];
}

static inline UINT default_compile_flags()
{
#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT const compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT const compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	return compileFlags;
}

ID3DBlob* graphics::vertex_shader(wchar_t const* const name)
{
	for (auto const& s : m_vertex_shaders)
	{
		if (s.first == name)
			return s.second.Get();
	}

	auto& shader = m_vertex_shaders[m_vertex_shaders_count++];
	shader.first = name;

	ComPtr<ID3DBlob> errors;

	auto const render_objects_count_string = std::to_string(render_object_instances_count);

	D3D_SHADER_MACRO const shader_macros[] = {
		{ "RENDER_OBJECT_INSTANCES_COUNT", render_objects_count_string.c_str() },
		{ nullptr, nullptr }
	};

	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(name, shader_macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_1", compile_flags, 0, &shader.second, &errors), errors);
	
	return shader.second.Get();
}

ID3DBlob* graphics::pixel_shader(wchar_t const* const name)
{
	for (auto const& s : m_pixel_shaders)
	{
		if (s.first == name)
			return s.second.Get();
	}
	auto& shader = m_pixel_shaders[m_pixel_shaders_count++];
	shader.first = name;

	ComPtr<ID3DBlob> errors;

	auto const render_objects_count_string = std::to_string(render_object_instances_count);

	D3D_SHADER_MACRO const shader_macros[] = {
		{ "RENDER_OBJECT_INSTANCES_COUNT", render_objects_count_string.c_str() },
		{ nullptr, nullptr }
	};

	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(name, shader_macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_1", compile_flags, 0, &shader.second, &errors), errors);

	return shader.second.Get();
}

bool graphics::initialize(HWND main_window_handle)
{
	increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, _countof(m_per_frame_model_view_projections) + _countof(m_per_frame_colors));

	for (auto& t : m_swap_chain_buffers)
		t.preinitialize(this, false, false, true);

	for (auto& t : m_indices_render_targets)
		t.preinitialize(this, false, false, true);
	
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

	ThrowIfFailed(m_d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_bundle_allocator)));

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
	g_current_frame_index = m_swap_chain->GetCurrentBackBufferIndex();
		
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
		clear_value.Format = indices_render_target_format;
		std::fill(std::begin(clear_value.Color), std::end(clear_value.Color), 0.0f);

		m_indices_render_targets[i].initialize(m_d3d_device.Get(), 0, g_options.screen_width, g_options.screen_height, 1, indices_render_target_format, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, &clear_value, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, L"render_target", i);
		m_indices_render_targets[i].initialize_rtv(m_d3d_device.Get(), indices_render_target_format);
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

render_object* graphics::new_render_object(
	render_object_owner* const owner,
	ID3D12PipelineState* const pipeline_state,
	ID3D12RootSignature* const root_signature,
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
	D3D12_VERTEX_BUFFER_VIEW const* instance_vertex_buffer_view,
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
	math::float4x4 const& view_projection_transform,
	math::float4x4 const* const model_transforms,
	math::float4 const* const colors,
	unsigned const instances_count
)
{
	m_render_objects[m_render_objects_count].initialize(
		owner,
		m_d3d_device.Get(),
		m_bundle_allocator.Get(),
		pipeline_state,
		root_signature,
		vertex_buffer_view,
		instance_vertex_buffer_view,
		index_buffer_view,
		primitive_topology,
		&m_render_object_instances[m_render_object_instances_count],
		instances_count
	);

	math::float4x4 model_view_projection;
	for (unsigned i = 0; i < instances_count; ++i)
	{
		math::float4x4 const model = model_transforms[i];
		m_render_object_instances[m_render_object_instances_count].initialize(m_render_objects[m_render_objects_count], model, colors[i]);

		math::multiply(model, view_projection_transform, model_view_projection);
		for (unsigned j = 0; j < frames_count; ++j)
		{
			update_render_object_model_view_projection(model_view_projection, m_render_object_instances_count, j);
			update_render_object_color(colors[i], m_render_object_instances_count, j);
		}
		m_render_object_instances_count++;
		assert(m_render_object_instances_count < render_object_instances_count);
	}

	render_object& result = m_render_objects[m_render_objects_count++];
	assert(m_render_objects_count < render_objects_count);
	return &result;
}