#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include "math.h"
#include "render_object.h"
#include "global_defines.h"
#include <utility>
#include <string>
#include "assets.h"
#include <vector>
#include <map>
#include "graphics_callbacks.h"

using Microsoft::WRL::ComPtr;

class logic;

class constant_buffer_data
{
public:
	constant_buffer_data() = default;
	constant_buffer_data(void* const mapped_data, D3D12_GPU_DESCRIPTOR_HANDLE const gpu_handle) :
		m_mapped_data(static_cast<char*>(mapped_data)),
		gpu_handle(gpu_handle)
	{}
	void update(unsigned const offset, void const* const data, unsigned const data_size)
	{
		memcpy(m_mapped_data + offset, data, data_size);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle{ 0 };
private:
	char* m_mapped_data = nullptr;
};

class graphics
{
public:
										graphics				(logic* const logic, unsigned const screen_width, unsigned const screen_height);
										~graphics				();
			void						resize					(unsigned const screen_width, unsigned const screen_height);
			void						run						(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed, unsigned const screen_width, unsigned const screen_height);
			
	inline	ID3D12Device*				device					( ) const { return m_d3d_device.Get(); }
	inline	D3D12_CPU_DESCRIPTOR_HANDLE current_cpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_cpu_handle[type]; }
	inline	D3D12_GPU_DESCRIPTOR_HANDLE current_gpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_gpu_handle[type]; }
			void						increment_handles		(D3D12_DESCRIPTOR_HEAP_TYPE const type);

	inline	void						increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE const type, unsigned const value) { m_descriptor_heap_sizes[type] += value; }

			void						select_object			(math::rectangle<math::uint2> const selection, unsigned const screen_width);
			void						highlight_object		(math::rectangle<math::uint2> const selection, unsigned const screen_width);
			void						remove_all_highlighting	();
			void						remove_all_selection	();
			void						new_render_object(
											unsigned const pipeline_state_id,
											unsigned const root_signature_id,
											unsigned const vertex_buffer_view_id,
											unsigned const instance_vertex_buffer_view_id,
											unsigned const index_buffer_view_id,
											D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
											math::float4x4 const* const model_transforms,
											math::float4 const* const colors,
											unsigned const instances_count,
											selection_updated_callback_type selection_updated_callback
										);

			constant_buffer_data		create_constant_buffer_view(unsigned const buffer_size);

			unsigned					pipeline_state_id(
											unsigned const root_signature_id, 
											D3D12_PRIMITIVE_TOPOLOGY_TYPE const primitive_topology_type,
											D3D12_INPUT_LAYOUT_DESC const& input_layout_description,
											wchar_t const* const vertex_shader_name,
											wchar_t const* const pixel_shader_name
										);
			inline ID3D12PipelineState*	pipeline_state(unsigned const id) const { return m_pipeline_states[id].Get(); }
			unsigned					root_signature_id();
			inline ID3D12RootSignature*	root_signature(unsigned const id) const { return m_root_signatures[id].Get(); }
			template<typename T, unsigned Size>
			unsigned					vertex_buffer_view_id(T (&vertices)[Size])
			{
				return vertex_buffer_view_id_impl(vertices, sizeof(vertices), sizeof(T));
			}
			template<typename T, unsigned Size>
			unsigned					index_buffer_view_id(T(&indices)[Size])
			{
				return index_buffer_view_id_impl(indices, sizeof(indices), std::is_same<unsigned, T>::value ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
			}
			ID3DBlob*					vertex_shader			(wchar_t const* const name );
			ID3DBlob*					pixel_shader			(wchar_t const* const name );

	inline	D3D12_VERTEX_BUFFER_VIEW&	vertex_buffer_view(unsigned const id) { return m_vertex_buffer_views[id]; }
	inline	D3D12_INDEX_BUFFER_VIEW&	index_buffer_view(unsigned const id) { return m_index_buffer_views[id]; }

			void						update_model_transforms(unsigned const current_frame_index);
			void						update_colors(unsigned const current_frame_index);
			void						update_view_projection_transform(math::float4x4 const& view_projection_transform, unsigned const current_frame_index);
			
private:
			void						create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask);

			void						update					(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed);
			void						initialize_constant_buffers();

			unsigned					vertex_buffer_view_id_impl(void const* const vertices, unsigned const vertices_size, unsigned const vertex_size);
			unsigned					index_buffer_view_id_impl(void const* const indices, unsigned const indices_size, DXGI_FORMAT const format);
			
private:
	HWND								m_main_window_handle;

	ComPtr<ID3D12Device>				m_d3d_device;
	ComPtr<ID3D12CommandAllocator>		m_bundle_allocator;

	ComPtr<IDXGISwapChain3>				m_swap_chain;
	unsigned							m_current_frame_index = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_swap_chain_rtv_cpu_handles[frames_count];
	ComPtr<ID3D12Resource>				m_swap_chain_buffers[frames_count];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_indices_rtv_cpu_handles[frames_count];
	ComPtr<ID3D12Resource>				m_indices_render_targets[frames_count];
	ComPtr<ID3D12Resource>				m_indices_rtv_readback_resource[frames_count];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_depth_stencil_dsv_cpu_handles[frames_count];
	ComPtr<ID3D12Resource>				m_depth_stencils[frames_count];
	unsigned							m_frame_id = 0;

	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fence_values[frames_count]{ 0 };
	UINT64								m_fence_value = 0;
	HANDLE								m_fence_event;

	ComPtr<ID3D12CommandQueue>			m_command_queue;
	ComPtr<ID3D12CommandAllocator>		m_direct_command_list_allocators[frames_count];
	ComPtr<ID3D12GraphicsCommandList>	m_command_lists[frames_count];

	std::vector<render_object>			m_render_objects;

	logic* const						m_logic;

	math::float4x4						m_model_transforms[render_object_instances_count];
	math::float4						m_colors[render_object_instances_count];
	bool								m_selected_object_instances[render_object_instances_count]{ false };
	bool								m_highlighted_object_instances[render_object_instances_count]{ false };
	selection_updated_callback_type		m_selection_updated_callbacks[render_object_instances_count];
	unsigned							m_render_object_instances_count = 0;

		
	D3D12_VIEWPORT	m_screen_viewport;
	D3D12_RECT		m_scissor_rectangle;
		
	ComPtr<ID3D12DescriptorHeap>	m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	unsigned int					m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE m_current_cpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE m_current_gpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	
	std::vector<ComPtr<ID3D12PipelineState>> m_pipeline_states;
	std::vector<ComPtr<ID3D12RootSignature>> m_root_signatures;
	std::vector<D3D12_VERTEX_BUFFER_VIEW>	m_vertex_buffer_views;
	std::vector<D3D12_INDEX_BUFFER_VIEW>	m_index_buffer_views;

	std::map<std::wstring, ComPtr<ID3DBlob>> m_vertex_shaders;
	std::map<std::wstring, ComPtr<ID3DBlob>> m_pixel_shaders;
private:

private:
	constant_buffer_data m_per_frame_model_transforms[frames_count];
	constant_buffer_data m_per_frame_colors[frames_count];

	DXGI_FORMAT const indices_render_target_format = DXGI_FORMAT_R32_UINT;

private:

	enum {
		upload_buffer_size = 1024 * 1024,
	};
	ComPtr<ID3D12Resource> m_upload_buffer_resource;
	char* m_upload_buffer_data_begin = nullptr;
	char* m_upload_buffer_data_current = nullptr;
	char* m_upload_buffer_data_end = nullptr;

}; // class graphics

#endif // #ifndef GRAPHICS_H_INCLUDED