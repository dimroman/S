#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <vector>
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "texture.h"
#include "sampler.h"
#include "math.h"
#include "camera.h"
#include "render_object.h"
#include "rectangle.h"
#include "square_grid.h"

using Microsoft::WRL::ComPtr;

class graphics
{
public:
										graphics				();

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
			void						add_render_object		(render_object* const object);
			
protected:
			void						create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask);
private:
			void						update					(float const last_frame_time);
			
private:

	ComPtr<ID3D12Device>				m_d3d_device;

	ComPtr<IDXGISwapChain3>				m_swap_chain;
	texture								m_swap_chain_buffers[frames_count];
	texture								m_indices_render_targets[frames_count];
	int									m_current_frame_index = 0;
	unsigned							m_frame_id = 0;

	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fence_values[frames_count]{ 0 };
	UINT64								m_fence_value = 0;
	HANDLE								m_fence_event;

	ComPtr<ID3D12CommandQueue>			m_command_queue;
	ComPtr<ID3D12CommandAllocator>		m_direct_command_list_allocators[frames_count];
	ComPtr<ID3D12GraphicsCommandList>	m_command_lists[frames_count];

	render_object						m_empty_render_object;
	
	enum { render_objects_count = 1024, };
	render_object*						m_render_objects[render_objects_count];
	unsigned int						m_render_objects_count = 0;

	render_object*						m_selected_render_object = &m_empty_render_object;
	render_object*						m_highlighted_render_object = &m_empty_render_object;
		
	D3D12_VIEWPORT	m_screen_viewport;
	D3D12_RECT		m_scissor_rectangle;
		
	ComPtr<ID3D12DescriptorHeap>	m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	unsigned int					m_descriptor_heap_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE m_current_cpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE m_current_gpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	
	UINT m_descriptor_sizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	
private:
	static inline float cell_side_length() { return 1.0f; }

private:
	enum {
		field_width = 8,
		field_height = 8,
	};
	rectangle							m_grid_cells[field_width*field_height];
	square_grid							m_square_grid;
}; // class graphics

#endif // #ifndef GRAPHICS_H_INCLUDED