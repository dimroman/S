#ifndef CONSTANT_BUFFER_H_INCLUDED
#define CONSTANT_BUFFER_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class graphics;

class constant_buffer
{
public:
	void preinitialize(graphics* const owner);
	void initialize(ID3D12Device* device, unsigned const buffer_size);

	inline D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle() const { return m_gpu_handle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle() const { return m_cpu_handle; }

	void update(void* data, unsigned const data_size);
private:
	ComPtr<ID3D12Resource> m_resource;

	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle{ 0 };

	graphics* m_owner;
};

#endif // #ifndef CONSTANT_BUFFER_H_INCLUDED