#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class graphics;

class buffer
{
public:
	void initialize(ID3D12Device* device, int const elements_count, UINT const element_size);
	inline D3D12_GPU_VIRTUAL_ADDRESS location() const { return m_resource->GetGPUVirtualAddress(); }

protected:
	UINT m_element_size;
	ComPtr<ID3D12Resource> m_resource;
};

#endif // BUFFER_H_INCLUDED