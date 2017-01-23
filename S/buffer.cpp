#include "buffer.h"
#include "graphics.h"
#include "helper_functions.h"
#include "d3dx12.h"

void buffer::initialize(ID3D12Device* device, int const elements_count, UINT const element_size)
{
	m_element_size = element_size;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_element_size * elements_count),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_resource)));
}