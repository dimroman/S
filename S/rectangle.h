#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "constant_buffer.h"
#include "render_object.h"

class graphics;
class vertex_buffer;

class rectangle : public render_object
{
public:
	void preinitialize(graphics* const owner);
	void initialize(graphics* const owner, ID3D12Device* const device, ID3D12GraphicsCommandList* const command_list, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, vertex_buffer* const vertex_buffer, math::float2 const position, float const width, float const height);
	void update(int const current_frame_index, D3D12_GPU_DESCRIPTOR_HANDLE const& per_frame_constants_handle);
private:
	struct static_constants
	{
		math::float2 position;
		float object_width;
		float object_height;
		unsigned int id;
	};
	constant_buffer m_static_constants_buffer;

	struct per_frame_constants
	{
		math::float4 color;
	};
	constant_buffer m_per_frame_constants[frames_count];
};

#endif // #ifndef RECTANGLE_H_INCLUDED