#include "square_grid.h"
#include "graphics.h"

extern unsigned g_index;

void square_grid::initialize(
	graphics* const owner,
	unsigned const width,
	unsigned const height,
	float const cell_side_length
)
{
	unsigned const vertices_count = 2 * (width + 1 + height + 1);
	unsigned const vertices_size = vertices_count * sizeof(math::float2);
	math::float2*	vertices = static_cast<math::float2*>(malloc(vertices_size));
	unsigned		index{ 0 };

	for (unsigned i = 0; i <= width; ++i)
	{
		vertices[index++] = { (float)i - width / 2.f, -(height / 2.f) };
		vertices[index++] = { (float)i - width / 2.f, height / 2.f };
	}
	for (unsigned i = 0; i <= height; ++i)
	{
		vertices[index++] = { -(width / 2.f), (float)i - height / 2.f };
		vertices[index++] = { width / 2.f, (float)i - height / 2.f };
	}
	assert(index == vertices_count);

	per_object_constants object_constants;

	object_constants.color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);
	object_constants.position = { 0.0f, 0.0f };
	object_constants.width = cell_side_length;
	object_constants.height = cell_side_length;

	super::initialize(
		owner,
		owner->pipeline_state(pipeline_states::line_one),
		owner->root_signature(root_signatures::one),
		owner->vertex_buffer_view(vertices, vertices_size, sizeof(math::float2), g_index++),
		nullptr, 
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		object_constants
	);
}