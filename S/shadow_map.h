#ifndef SHADOW_MAP_H_INCLUDED
#define SHADOW_MAP_H_INCLUDED

#include "options.h"
#include "constant_buffer.h"
#include <functional>
#include "texture2d.h"
#include "defines.h"
#include "math.h"
#include "sampler.h"

extern options g_options;

class graphics;
class world_camera;

class shadow_map
{
public:
	shadow_map() = default;
	void initialize(ID3D12Device* const device, graphics* const heaps_owner);
	void update(int const current_frame_index, world_camera& light_camera);
	void draw(int const current_frame_index, ID3D12GraphicsCommandList* command_list, std::function<void()> draw_callback);
	D3D12_GPU_DESCRIPTOR_HANDLE inline shadow_resource(int const current_frame_index) const { return m_textures[current_frame_index].gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); }

private:
	ComPtr<ID3D12RootSignature> m_root_signature;
	ComPtr<ID3D12PipelineState> m_pipeline_state;

	D3D12_RESOURCE_BARRIER m_start_transitions[frames_count];
	D3D12_RESOURCE_BARRIER m_end_transitions[frames_count];

	texture2d m_textures[frames_count];
	struct shadows_constant_buffer
	{
		math::float4x4 model_view_projection;
		float float_padding[48];
	};
	constant_buffer<shadows_constant_buffer> m_constant_buffers[frames_count];
};

#endif // #ifndef SHADOW_MAP_H_INCLUDED