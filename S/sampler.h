#ifndef SAMPLER_H_INCLUDED
#define SAMPLER_H_INCLUDED

#include <d3d12.h>

class graphics;

class sampler
{
public:
	void initialize(ID3D12Device* const device, graphics* const heaps_owner, D3D12_FILTER const filter, D3D12_TEXTURE_ADDRESS_MODE const address_u, D3D12_TEXTURE_ADDRESS_MODE const address_v, D3D12_TEXTURE_ADDRESS_MODE const address_w);

	inline D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle() const { return m_gpu_handle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle() const { return m_cpu_handle; }
private:
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle{ 0 };
};

#endif SAMPLER_H_INCLUDED