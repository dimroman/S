#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include "texture.h"
#include "sampler.h"
#include "math.h"
#include "render_object.h"
#include "square_field.h"
#include "hexagon_field.h"
#include <utility>

using Microsoft::WRL::ComPtr;

class graphics
{
public:
			bool						initialize				(HWND main_window_handle);
			void						run						(float const last_frame_time);
			void						finalize				();

			UINT						descriptor_size			(D3D12_DESCRIPTOR_HEAP_TYPE type);

	inline	ID3D12Device*				device					( ) const { return m_d3d_device.Get(); }
	inline	D3D12_CPU_DESCRIPTOR_HANDLE current_cpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_cpu_handle[type]; }
	inline	D3D12_GPU_DESCRIPTOR_HANDLE current_gpu_handle		(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_gpu_handle[type]; }
			void						increment_handles		(D3D12_DESCRIPTOR_HEAP_TYPE const type);

	inline	void						increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE const type, unsigned const value) { m_descriptor_heap_sizes[type] += value; }

			void						select_object			(int const x, int const y);
			void						highlight_object		(int const x, int const y);
			render_object*				new_render_object(
											render_object_owner* const owner,
											ID3D12PipelineState* const pipeline_state,
											ID3D12RootSignature* const root_signature,
											D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
											D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
											D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
											per_object_constants const& object_constants
										);
			constant_buffer_data		create_constant_buffer_view(unsigned const buffer_size);

			ID3D12PipelineState*		pipeline_state			(unsigned const id);
			ID3D12RootSignature*		root_signature			(unsigned const id);
			D3D12_VERTEX_BUFFER_VIEW*	vertex_buffer_view		(void const* const vertices, unsigned const vertices_size, unsigned const vertex_size, unsigned const index);
			D3D12_INDEX_BUFFER_VIEW*	index_buffer_view		(void const* const indices, unsigned const indices_size, DXGI_FORMAT const format, unsigned const index);

			void						update_render_object(per_object_constants const& object_constants, unsigned const id, unsigned const current_frame_index);
			
private:
			void						create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask);

			void						update					(float const last_frame_time);
			void						initialize_constant_buffers();
			
private:

	ComPtr<ID3D12Device>				m_d3d_device;

	ComPtr<IDXGISwapChain3>				m_swap_chain;
	texture								m_swap_chain_buffers[frames_count];
	texture								m_indices_render_targets[frames_count];
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

	unsigned							m_selected_render_object_id = 0;
	unsigned							m_highlighted_render_object_id = 0;
		
	D3D12_VIEWPORT	m_screen_viewport;
	D3D12_RECT		m_scissor_rectangle;
		
	ComPtr<ID3D12DescriptorHeap>	m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	unsigned int					m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE m_current_cpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE m_current_gpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	
	UINT m_descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	ComPtr<ID3D12PipelineState> m_pipeline_states[pipeline_states::count];
	ComPtr<ID3D12RootSignature> m_root_signatures[root_signatures::count];
	std::pair<D3D12_VERTEX_BUFFER_VIEW, unsigned> m_vertex_buffer_views[max_vertex_buffer_views_count];
	unsigned m_vertex_buffer_views_count{ 0 };
	std::pair<D3D12_INDEX_BUFFER_VIEW, unsigned> m_index_buffer_views[max_index_buffer_views_count];
	unsigned m_index_buffer_views_count{ 0 };	
private:

private:
	constant_buffer_data m_per_frame_constants[frames_count];

	//hexagon_field m_field;
	square_field m_field;
}; // class graphics

#endif // #ifndef GRAPHICS_H_INCLUDED