#include "texture.h"
#include "helper_functions.h"
#include "d3dx12.h"
#include "graphics.h"

void texture::preinitialize(graphics* const owner, bool const use_as_cbv_srv_uav, bool const use_as_dsv, bool const use_as_rtv)
{
	m_owner = owner;

	if ( use_as_cbv_srv_uav )
		m_owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

	if (use_as_dsv)
		m_owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

	if (use_as_rtv)
		m_owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
}

void texture::initialize( ID3D12Device* device, UINT64 const alignment, UINT64 const width, UINT const height, UINT16 const mip_levels, DXGI_FORMAT const format, DXGI_SAMPLE_DESC const sample_desc, D3D12_TEXTURE_LAYOUT const layout,
	D3D12_CLEAR_VALUE const* const clear_value, D3D12_RESOURCE_STATES const state, D3D12_RESOURCE_FLAGS const flags, LPCWSTR name, UINT const index)
{
	m_description.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	m_description.Alignment = alignment;
	m_description.Width = width;
	m_description.Height = height;
	m_description.DepthOrArraySize = 1;
	m_description.MipLevels = mip_levels;
	m_description.Format = format;
	m_description.SampleDesc = sample_desc;
	m_description.Layout = layout;
	m_description.Flags = flags;
	
	D3D12_HEAP_PROPERTIES heap_properties{};
	heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	ThrowIfFailed(device->CreateCommittedResource( 
		&heap_properties, 
		D3D12_HEAP_FLAG_NONE, 
		&m_description, 
		state, 
		clear_value, 
		IID_PPV_ARGS(m_resource.GetAddressOf()))
	);
	SetNameIndexed(m_resource.Get(), name, index);
}

void texture::initialize_with_data(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, int const row_pitch, int const slice_pitch, void* data)
{
	UINT64 const buffer_size = GetRequiredIntermediateSize(m_resource.Get(), 0, m_description.MipLevels);

	D3D12_HEAP_PROPERTIES heap_properties{};
	heap_properties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;

	ThrowIfFailed(device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_texture_upload_heap)));

	m_texture_data.pData = data;
	m_texture_data.RowPitch = row_pitch;
	m_texture_data.SlicePitch = slice_pitch;

	UpdateSubresources(command_list, m_resource.Get(), m_texture_upload_heap.Get(), 0, 0, m_description.MipLevels, &m_texture_data);
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

void texture::initialize_readback_resource(ID3D12Device* device)
{
	UINT64 const buffer_size = GetRequiredIntermediateSize(m_resource.Get(), 0, m_description.MipLevels);
	
	D3D12_HEAP_PROPERTIES heap_properties{};
	heap_properties.Type = D3D12_HEAP_TYPE_READBACK;
	heap_properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_properties.CreationNodeMask = 1;
	heap_properties.VisibleNodeMask = 1;
	
	ThrowIfFailed(device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_texture_readback_heap)));

}

void texture::update_readback_resource(ID3D12GraphicsCommandList* command_list)
{
	D3D12_TEXTURE_COPY_LOCATION destination;
	destination.pResource = m_texture_readback_heap.Get();
	destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	destination.PlacedFootprint.Offset = 0;
	destination.PlacedFootprint.Footprint.Depth = m_description.DepthOrArraySize;
	destination.PlacedFootprint.Footprint.Height = m_description.Height;
	destination.PlacedFootprint.Footprint.Width = static_cast<UINT>(m_description.Width);
	destination.PlacedFootprint.Footprint.Format = m_description.Format;
	destination.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(m_description.Width) * sizeof(UINT);

	D3D12_TEXTURE_COPY_LOCATION source;
	source.pResource = m_resource.Get();
	source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	source.SubresourceIndex = 0;

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));
	command_list->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr );
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
}

unsigned texture::readback_value(int const offset) const
{
	UINT64 const buffer_size = GetRequiredIntermediateSize(m_resource.Get(), 0, m_description.MipLevels);
	
	void* data = nullptr;
	D3D12_RANGE read_range{ offset*sizeof(unsigned),(offset+1)*sizeof(unsigned) };
	ThrowIfFailed(m_texture_readback_heap->Map(0, &read_range, &data));
	auto result = *(static_cast<unsigned*>(data) + offset);
	m_texture_readback_heap->Unmap(0, nullptr);
	return result;
}

void texture::initialize_cbv_srv_uav(ID3D12Device* device, DXGI_FORMAT const format)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MipLevels = m_description.MipLevels;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.ResourceMinLODClamp = 0.0f;

	device->CreateShaderResourceView(m_resource.Get(), &desc, m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	m_gpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = m_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_cpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void texture::initialize_dsv(ID3D12Device* device, DXGI_FORMAT const format)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	desc.Format = format;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	desc.Flags = D3D12_DSV_FLAG_NONE;
	desc.Texture2D = { 0 };
	device->CreateDepthStencilView(m_resource.Get(), &desc, m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV) );
	
	m_gpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = m_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	m_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void texture::initialize_rtv(ID3D12Device* device, DXGI_FORMAT const format)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc{};
	desc.Format = format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D = { 0, 0 };

	device->CreateRenderTargetView(m_resource.Get(), &desc, m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	m_gpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = m_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void texture::initialize_rtv(ID3D12Device* device, IDXGISwapChain3* swap_chain, int const buffer_id)
{
	ThrowIfFailed(swap_chain->GetBuffer(buffer_id, IID_PPV_ARGS(m_resource.GetAddressOf())));

	device->CreateRenderTargetView(m_resource.Get(), nullptr, m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	
	m_gpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = m_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	m_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}