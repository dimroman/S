#ifndef CONSTANT_BUFFER_H_INCLUDED
#define CONSTANT_BUFFER_H_INCLUDED

#include "buffer.h"

class graphics;

class constant_buffer_impl : public buffer
{
public:
	void initialize_cbv(ID3D12Device* device, graphics* const heaps_owner);

	inline D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle() const { return m_gpu_handle; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle() const { return m_cpu_handle; }

protected:
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle{ 0 };
};

template<typename BufferType>
class constant_buffer : public constant_buffer_impl
{
	static_assert(sizeof(BufferType) % 256 == 0, "sizeof(BufferType) % 256 != 0");
public:
	inline void initialize(ID3D12Device* device, int const elements_count) { buffer::initialize(device, elements_count, sizeof(BufferType)); }
	void update(BufferType const& data, int const index)
	{
		D3D12_RANGE read_range{ 0,0 };
		ThrowIfFailed(m_resource->Map(0, &read_range, reinterpret_cast<void**>(&m_constant_buffer_data)));
		memcpy(&m_constant_buffer_data[index], &data, sizeof(data));
		m_resource->Unmap(0, nullptr);
	}
private:
	BufferType* m_constant_buffer_data{ nullptr };
};

#endif // #ifndef CONSTANT_BUFFER_H_INCLUDED