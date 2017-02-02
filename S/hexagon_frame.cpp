#include "hexagon_frame.h"

#include "graphics.h"

extern unsigned g_index;

void hexagon_frame::initialize(
	graphics* const owner,
	math::float2 const position,
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

	per_object_constants object_constants;

	object_constants.color = { 0.1f, 0.2f, 0.3f, 1.0f };
	object_constants.position = position;
	object_constants.width = radii;
	object_constants.height = radii;

	super::initialize(
		owner,
		owner->pipeline_state(pipeline_states::line_one),
		owner->root_signature(root_signatures::one),
		owner->vertex_buffer_view(vertices, sizeof(vertices), sizeof(math::float2), index),
		owner->index_buffer_view(indices, sizeof(indices), DXGI_FORMAT_R32_UINT, index),
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		object_constants
	);
}