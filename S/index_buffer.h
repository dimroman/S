#ifndef INDEX_BUFFER_H_INCLUDED
#define INDEX_BUFFER_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class index_buffer
{
public:
	void initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, void* const buffer, UINT const buffer_size, DXGI_FORMAT const format);
	D3D12_INDEX_BUFFER_VIEW view() const { return m_view; }
private:
	D3D12_INDEX_BUFFER_VIEW m_view;
	ComPtr<ID3D12Resource> m_upload_heap;
	ComPtr<ID3D12Resource> m_resource;
	D3D12_SUBRESOURCE_DATA m_subresource_data{};
};

#endif // #ifndef INDEX_BUFFER_H_INCLUDED