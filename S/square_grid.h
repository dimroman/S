#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "math.h"
#include "vertex_buffer.h"
#include "constant_buffer.h"
#include "render_object.h"

class graphics;

class square_grid : public render_object
{
public:
	void preinitialize(graphics* const owner);
	void initialize	(graphics* const owner, ID3D12Device* device, ID3D12GraphicsCommandList* command_list, unsigned const width, unsigned const height, float const cell_side_length, math::float3 const color);
	void update		(int const current_frame_index);
	void draw		(ID3D12GraphicsCommandList* const command_list, int const current_frame_index);
private:
	enum { max_vertices_count = 1000, };
	unsigned		m_width{ 0 };
	unsigned		m_height{ 0 };

	float			m_cell_side_length{ 0.f };

	struct vertex
	{
		math::float2 position;
		math::float3 color;
	};
	vertex			m_vertices[max_vertices_count];
	unsigned		m_vertices_count{ 0 };

	vertex_buffer	m_vertex_buffer;

	struct transform_constants
	{
		math::float4x4 model_view_projection;
		unsigned int id;
		float float_padding[47];
	};

	constant_buffer m_transform_constants_buffer[frames_count];

	ComPtr<ID3D12RootSignature>			m_root_signature;
	ComPtr<ID3D12PipelineState>			m_pipeline_state;
};

#endif // #ifndef SQUARE_GRID_H_INCLUDED