#include "sampler.h"
#include "graphics.h"

void sampler::initialize(ID3D12Device* const device, graphics* const heaps_owner, D3D12_FILTER const filter, D3D12_TEXTURE_ADDRESS_MODE const address_u, D3D12_TEXTURE_ADDRESS_MODE const address_v, D3D12_TEXTURE_ADDRESS_MODE const address_w)
{
	D3D12_SAMPLER_DESC desc{};
	desc.Filter = filter;
	desc.AddressU = address_u;
	desc.AddressV = address_v;
	desc.AddressW = address_w;
	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D12_FLOAT32_MAX;
	device->CreateSampler(&desc, heaps_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER));

	m_gpu_handle = heaps_owner->current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	m_cpu_handle = heaps_owner->current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	heaps_owner->increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}