#ifndef RENDER_TARGET_H_INCLUDED
#define RENDER_TARGET_H_INCLUDED

#include "texture2d.h"
#include "d3dx12.h"
#include "vertex_buffer.h"
#include "sampler.h"
#include "defines.h"

enum { render_targets_count = 4, };

class graphics;

class g_buffer
{
public:
	void initialize(ID3D12Device* const device, ID3D12GraphicsCommandList* command_list, graphics* const heaps_owner, DXGI_FORMAT const formats[render_targets_count]);
	void clear(ID3D12GraphicsCommandList* command_list, int const current_frame_index);
	void draw(int const current_frame_index, ID3D12GraphicsCommandList* command_list, D3D12_GPU_DESCRIPTOR_HANDLE const& scene_constant_buffer, D3D12_GPU_DESCRIPTOR_HANDLE const& shadow_map, D3D12_GPU_DESCRIPTOR_HANDLE const& depth_buffer);
	inline D3D12_CPU_DESCRIPTOR_HANDLE render_target_resource(int const current_frame_index, int const id) const { return m_textures[current_frame_index*render_targets_count + id].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); }

private:
	ComPtr<ID3D12RootSignature> m_root_signature;
	ComPtr<ID3D12PipelineState> m_pipeline_state;

	D3D12_RESOURCE_BARRIER m_start_transitions[frames_count*render_targets_count];
	D3D12_RESOURCE_BARRIER m_end_transitions[frames_count*render_targets_count];

	texture2d	m_textures[render_targets_count*frames_count];
	D3D12_CLEAR_VALUE m_clear_values[render_targets_count];

	sampler m_clamp_sampler;
};

#endif // #ifndef RENDER_TARGET_H_INCLUDED