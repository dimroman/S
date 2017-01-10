#include "constant_buffer.h"
#include "graphics.h"

void constant_buffer_impl::initialize_cbv(ID3D12Device* device, graphics* const heaps_owner)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{ location(), m_element_size };
	device->CreateConstantBufferView(&cbv_desc, heaps_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	m_gpu_handle = heaps_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_cpu_handle = heaps_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	heaps_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}