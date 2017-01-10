#ifndef VERTEX_BUFFER_H_INCLUDED
#define VERTEX_BUFFER_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class vertex_buffer
{
public:
	void initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, void* const buffer, UINT const buffer_size, UINT const stride_in_bytes);
	D3D12_VERTEX_BUFFER_VIEW view() const { return m_view; }
private:
	D3D12_VERTEX_BUFFER_VIEW m_view;
	D3D12_SUBRESOURCE_DATA m_subresource_data;
	ComPtr<ID3D12Resource> m_resource;
	ComPtr<ID3D12Resource> m_upload_heap;
};

#endif // #ifndef VERTEX_BUFFER_H_INCLUDED