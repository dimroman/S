#ifndef TEXTURE2D_H_INCLUDED
#define TEXTURE2D_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_5.h>

using Microsoft::WRL::ComPtr;

class graphics;

class texture
{
public:
	texture();
	texture(graphics* const owner, bool const use_as_cbv_srv_uav, bool const use_as_dsv, bool const use_as_rtv );
	void initialize( ID3D12Device* device, UINT64 const alignment, UINT64 const width, UINT const height, UINT16 const mip_levels, DXGI_FORMAT const format, DXGI_SAMPLE_DESC const sample_desc, D3D12_TEXTURE_LAYOUT const layout,
		D3D12_CLEAR_VALUE const* const clear_value, D3D12_RESOURCE_STATES const state, D3D12_RESOURCE_FLAGS const flags, LPCWSTR name, UINT const index);
	void initialize_with_data( ID3D12Device* device, ID3D12GraphicsCommandList* command_list, int const row_pitch, int const slice_pitch, void* data);
	void initialize_readback_resource(ID3D12Device* device);
	void initialize_cbv_srv_uav(ID3D12Device* device, DXGI_FORMAT const format);
	void initialize_dsv(ID3D12Device* device, DXGI_FORMAT const format);
	void initialize_rtv(ID3D12Device* device, DXGI_FORMAT const format);
	void initialize_rtv(ID3D12Device* device, IDXGISwapChain3* swap_chain, int const buffer_id);

	void update_readback_resource(ID3D12GraphicsCommandList* command_list);
	unsigned readback_value(int const offset) const;

	ID3D12Resource* resource() { return m_resource.Get(); }
	inline D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_gpu_handles[type]; }
	inline D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_cpu_handles[type]; }
private:
	ComPtr<ID3D12Resource>		m_resource;
	D3D12_RESOURCE_DESC			m_description{};
	ComPtr<ID3D12Resource>		m_texture_upload_heap;
	ComPtr<ID3D12Resource>		m_texture_readback_heap;
	D3D12_SUBRESOURCE_DATA		m_texture_data{};

	D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	graphics* const				m_owner;
};

#endif // #ifndef TEXTURE2D_H_INCLUDED