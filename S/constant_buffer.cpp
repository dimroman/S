#include "constant_buffer.h"
#include "graphics.h"
#include "d3dx12.h"
#include "helper_functions.h"

void constant_buffer::preinitialize(graphics* const owner)
{
	m_owner = owner;
	m_owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
}

void constant_buffer::initialize(ID3D12Device* device, unsigned const buffer_size)
{
	assert(buffer_size % 256 == 0);

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_resource)));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{ m_resource->GetGPUVirtualAddress(), buffer_size };
	device->CreateConstantBufferView(&cbv_desc, m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	m_gpu_handle = m_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_cpu_handle = m_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void constant_buffer::update(void* data, unsigned const data_size)
{
	D3D12_RANGE read_range{ 0,0 };
	void* buffer_data = alloca(data_size);
	ThrowIfFailed(m_resource->Map(0, &read_range, &buffer_data));
	memcpy(buffer_data, data, data_size);
	m_resource->Unmap(0, nullptr);
}