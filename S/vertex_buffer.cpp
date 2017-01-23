#include "vertex_buffer.h"
#include "helper_functions.h"
#include "d3dx12.h"

void vertex_buffer::initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, void* const buffer, UINT const vertex_size, UINT const vertices_count)
{	
	m_vertex_size = vertex_size;
	m_vertices_count = vertices_count;

	auto const buffer_size = vertex_size*vertices_count;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(m_resource.GetAddressOf())));

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_upload_heap.GetAddressOf())));

	m_subresource_data.pData = buffer;
	m_subresource_data.RowPitch = buffer_size;
	m_subresource_data.SlicePitch = m_subresource_data.RowPitch;

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(command_list, m_resource.Get(), m_upload_heap.Get(), 0, 0, 1, &m_subresource_data);
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	m_view.BufferLocation = m_resource->GetGPUVirtualAddress();
	m_view.SizeInBytes = buffer_size;
	m_view.StrideInBytes = vertex_size;
}