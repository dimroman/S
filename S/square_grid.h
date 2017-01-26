#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "math.h"
#include "render_object.h"

class graphics;

class square_grid : public render_object
{
public:
	void preinitialize(graphics* const owner);
	void initialize	(graphics* const owner, ID3D12Device* const device, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, float const cell_side_length);
	void update		(int const current_frame_index);
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

#endif // #ifndef SQUARE_GRID_H_INCLUDED