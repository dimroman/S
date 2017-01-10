#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <vector>
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "constant_buffer.h"
#include "texture2d.h"
#include "assets.h"
#include "shadow_map.h"
#include "g_buffer.h"
#include "sampler.h"
#include "defines.h"
#include "math.h"
#include "camera.h"

using Microsoft::WRL::ComPtr;

class graphics
{
public:
										graphics				();

			bool						initialize				(HWND main_window_handle);
			void						run						(float const last_frame_time);
			void						finalize				();

	static	UINT						descriptor_size			(D3D12_DESCRIPTOR_HEAP_TYPE type);

	inline	D3D12_CPU_DESCRIPTOR_HANDLE current_cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_cpu_handle[type]; }
	inline	D3D12_GPU_DESCRIPTOR_HANDLE current_gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE const type) const { return m_current_gpu_handle[type]; }
			void						increment_handles(D3D12_DESCRIPTOR_HEAP_TYPE const type);


public:
			static const UINT NumLights = 3;		// Keep this in sync with "shaders.hlsl".

protected:
			void						create_descriptor_heap(ID3D12Device* const device, D3D12_DESCRIPTOR_HEAP_TYPE const type, UINT num_descriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT node_mask);
private:
			void						update					(float const last_frame_time);
			
private:
	ComPtr<ID3D12RootSignature>			m_root_signature;
	ComPtr<ID3D12PipelineState>			m_pipeline_state;

	D3D12_RESOURCE_BARRIER				m_start_transitions[frames_count];
	D3D12_RESOURCE_BARRIER				m_end_transitions[frames_count];

	ComPtr<ID3D12Device>				m_d3d_device;

	ComPtr<IDXGISwapChain3>				m_swap_chain;
	texture2d							m_swap_chain_buffers[frames_count];
	int									m_current_frame_index = 0;

	ComPtr<ID3D12Fence>					m_fence;
	UINT64								m_fence_values[frames_count]{ 0 };
	UINT64								m_fence_value = 0;
	HANDLE								m_fence_event;

	ComPtr<ID3D12CommandQueue>			m_command_queue;
	ComPtr<ID3D12CommandAllocator>		m_direct_command_list_allocators[frames_count];
	ComPtr<ID3D12GraphicsCommandList>	m_command_lists[frames_count];

	texture2d							m_depth_stencils[frames_count];

	sampler								m_wrap_sampler;
		
	D3D12_VIEWPORT	m_screen_viewport;
	D3D12_RECT		m_scissor_rectangle;

	vertex_buffer m_vertex_buffer;
	index_buffer m_index_buffer;
	texture2d m_textures[_countof(Textures)];

	struct LightState
	{
		math::float4 position;
		math::float4 direction;
		math::float4 color;
		math::float4 falloff;
		math::float4x4 view_projection;
	};

	struct g_buffer_constants
	{
		math::float4x4 view_inverted;
		math::float4 perspective_values;
		math::float4 ambientColor;
		LightState lights[NumLights];
		unsigned sampleShadowMap;
		float screen_width;
		float screen_height;
		float float_padding[5];
	};
	struct transform_constants_buffer
	{
		math::float4x4 model;
		math::float4x4 view_projection;
		float float_padding[32];
	};

	constant_buffer<transform_constants_buffer> m_transform_constants_buffers[frames_count];
	constant_buffer<g_buffer_constants>			m_g_buffer_constants[frames_count];

	LightState	m_lights[NumLights];	
	world_camera m_light_cameras[NumLights];
	
	shadow_map			m_shadow_map;
	g_buffer			m_g_buffer;

	ComPtr<ID3D12DescriptorHeap> m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	D3D12_CPU_DESCRIPTOR_HANDLE m_current_cpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE m_current_gpu_handle[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]{ 0 };
}; // class graphics

#endif // #ifndef GRAPHICS_H_INCLUDED