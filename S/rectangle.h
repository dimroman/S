#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "render_object.h"

class graphics;

class rectangle : public render_object
{
public:
	void preinitialize(graphics* const owner);
	void initialize(graphics* const owner, ID3D12Device* const device, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, math::float2 const position, float const width, float const height);
	void update(int const current_frame_index);
private:
	struct static_constants
	{
		math::float2 position;
		float object_width;
		float object_height;
		unsigned int id;
	};
	constant_buffer_data m_static_constants;

	struct per_frame_constants
	{
		math::float4 color;
	};
	constant_buffer_data m_per_frame_constants[frames_count];
};

#endif // #ifndef RECTANGLE_H_INCLUDED