#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include "texture.h"
#include "sampler.h"
#include "math.h"
#include "render_object.h"
#include "global_defines.h"
#include <utility>
#include <string>
#include "assets.h"

using Microsoft::WRL::ComPtr;

class render_object_instance_owner;

class graphics
{
public:
			bool						initialize				(HWND main_window_handle);
			void						run						(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed);
			void						finalize				();

			UINT						descriptor_size			(D3D12_DESCRIPTOR_HEAP_TYPE type);

	inline	ID3D12Device*				device					( ) const { return m_d3d_device.Get(); }
	inline	D3D12_CPU_DESCRIPTOR_HANDLE current_cpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_cpu_handle[type]; }
	inline	D3D12_GPU_DESCRIPTOR_HANDLE current_gpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_gpu_handle[type]; }
			void						increment_handles		(D3D12_DESCRIPTOR_HEAP_TYPE const type);

	inline	void						increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE const type, unsigned const value) { m_descriptor_heap_sizes[type] += value; }

			void						select_object			(math::rectangle<math::int2> const selection);
			void						highlight_object		(math::rectangle<math::int2> const selection);
			void						remove_all_highlighting	();
			void						remove_all_selection	();
			render_object*				new_render_object(
											render_object_instance_owner** const render_object_instance_owners,
											ID3D12PipelineState* const pipeline_state,
											ID3D12RootSignature* const root_signature,
											D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
											D3D12_VERTEX_BUFFER_VIEW const* instance_vertex_buffer_view,
											D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
											D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
											math::float4x4 const* const model_transforms,
											math::float4 const* const colors,
											unsigned const instances_count,
											unsigned& out_first_render_object_instance_id
										);

			void						update_model_transform(math::float4x4 const& model_transform, unsigned const render_object_instance_id);
			void						update_color(math::float4 const& color, unsigned const render_object_instance_id);
			constant_buffer_data		create_constant_buffer_view(unsigned const buffer_size);

			ID3D12PipelineState*		pipeline_state			(
											ID3D12RootSignature* const root_signature, 
											D3D12_PRIMITIVE_TOPOLOGY_TYPE const primitive_topology_type,
											D3D12_INPUT_LAYOUT_DESC const& input_layout_description,
											ID3DBlob* const vertex_shader,
											ID3DBlob* const pixel_shader
										);
			ID3D12RootSignature*		root_signature			();
			template<typename T, unsigned Size>
			D3D12_VERTEX_BUFFER_VIEW*	vertex_buffer_view(T (&vertices)[Size])
			{
				return vertex_buffer_view_impl(vertices, sizeof(vertices), sizeof(T));
			}
			template<typename T, unsigned Size>
			D3D12_INDEX_BUFFER_VIEW*	index_buffer_view(T(&indices)[Size])
			{
				return index_buffer_view_impl(indices, sizeof(indices), std::is_same<unsigned, T>::value ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_UNKNOWN);
			}
			ID3DBlob*					vertex_shader			(wchar_t const* const name );
			ID3DBlob*					pixel_shader			(wchar_t const* const name );

			void						update_model_transforms(unsigned const current_frame_index);
			void						update_colors(unsigned const current_frame_index);
			void						update_view_projection_transform(math::float4x4 const& view_projection_transform, unsigned const current_frame_index);
			
private:
			void						create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask);

			void						update					(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed);
			void						initialize_constant_buffers();

			D3D12_VERTEX_BUFFER_VIEW*	vertex_buffer_view_impl(void const* const vertices, unsigned const vertices_size, unsigned const vertex_size);
			D3D12_INDEX_BUFFER_VIEW*	index_buffer_view_impl(void const* const indices, unsigned const indices_size, DXGI_FORMAT const format);
			
private:

	ComPtr<ID3D12Device>				m_d3d_device;
	ComPtr<ID3D12CommandAllocator>		m_bundle_allocator;

	ComPtr<IDXGISwapChain3>				m_swap_chain;
	texture								m_swap_chain_buffers[frames_count];
	texture								m_indices_render_targets[frames_count];
	texture								m_depth_stencils[frames_count];
	unsigned							m_frame_id = 0;

	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fence_values[frames_count]{ 0 };
	UINT64								m_fence_value = 0;
	HANDLE								m_fence_event;

	ComPtr<ID3D12CommandQueue>			m_command_queue;
	ComPtr<ID3D12CommandAllocator>		m_direct_command_list_allocators[frames_count];
	ComPtr<ID3D12GraphicsCommandList>	m_command_lists[frames_count];

	render_object						m_render_objects[render_objects_count];
	unsigned							m_render_objects_count = 0;

	render_object_instance_owner*		m_render_object_instance_owners[render_object_instances_count];
	math::float4x4						m_model_transforms[render_object_instances_count];
	math::float4						m_colors[render_object_instances_count];
	unsigned							m_render_object_instances_count = 0;

	bool m_selected_object_instances[render_object_instances_count];
	bool m_highlighted_object_instances[render_object_instances_count];
		
	D3D12_VIEWPORT	m_screen_viewport;
	D3D12_RECT		m_scissor_rectangle;
		
	ComPtr<ID3D12DescriptorHeap>	m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	unsigned int					m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE m_current_cpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE m_current_gpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	
	UINT m_descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	ComPtr<ID3D12PipelineState> m_pipeline_states[max_pipeline_states_count];
	unsigned m_pipeline_states_count = 0;
	ComPtr<ID3D12RootSignature> m_root_signatures[max_root_signatures_count];
	unsigned m_root_signatures_count = 0;
	D3D12_VERTEX_BUFFER_VIEW m_vertex_buffer_views[max_vertex_buffer_views_count];
	unsigned m_vertex_buffer_views_count = 0;
	D3D12_INDEX_BUFFER_VIEW m_index_buffer_views[max_index_buffer_views_count];
	unsigned m_index_buffer_views_count = 0;

	std::pair<std::wstring, ComPtr<ID3DBlob>> m_vertex_shaders[max_vertex_shaders_count];
	unsigned m_vertex_shaders_count = 0;

	std::pair<std::wstring, ComPtr<ID3DBlob>> m_pixel_shaders[max_pixel_shaders_count];
	unsigned m_pixel_shaders_count = 0;
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