#include "hexagon_frame.h"

#include "graphics.h"

extern unsigned g_index;

void hexagon_frame::initialize(
	graphics* const graphics,
	math::float4x4 const& view_projection,
	math::float2 const& position,
	float const radii
)
{
	static unsigned index = g_index++;

	math::float2 vertices[]{
		{ 0.0f, 1.0f },
		{ 0.866025f, 0.5f },
		{ 0.866025f, -0.5f },
		{ 0.0f, -1.0f },
		{ -0.866025f, -0.5f },
		{ -0.866025f, 0.5f }
	};

	unsigned indices[]{
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0
	};

	math::float4 const color = { 0.1f, 0.2f, 0.3f, 1.0f };
	math::float4x4 model_transform = math::float4x4::identity();
	model_transform.m[0][0] = radii;
	model_transform.m[0][0] = radii;
	model_transform.m[0][3] = position.x;
	model_transform.m[1][3] = position.y;

	m_render_object = graphics->new_render_object(
		this,
		graphics->pipeline_state(pipeline_states::line_one),
		graphics->root_signature(root_signatures::one),
		graphics->vertex_buffer_view(vertices, sizeof(vertices), sizeof(math::float2), index),
		graphics->index_buffer_view(indices, sizeof(indices), DXGI_FORMAT_R32_UINT, index),
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		model_transform,
		view_projection,
		color
	);
}