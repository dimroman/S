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

bool graphics::initialize(HWND main_window_handle)
{
	for (auto& cell : m_grid_cells)
		cell.preinitialize(this);
	m_square_grid.preinitialize(this);

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
	
	m_render_objects[m_render_objects_count++] = &m_empty_render_object;

	for ( int i = 0; i < field_width; i++ )
	for ( int j = 0; j < field_height; j++ )
		m_grid_cells[i + j * field_width].initialize(this, m_d3d_device.Get(), initialization_command_list.Get(), { cell_side_length()*i - cell_side_length()*(field_width-1)/2, cell_side_length()*j - cell_side_length()*(field_height-1)/2 }, cell_side_length(), cell_side_length());
	m_square_grid.initialize(this, m_d3d_device.Get(), initialization_command_list.Get(), field_width, field_height, cell_side_length(), { 0.1f, 0.2f, 0.3f });

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