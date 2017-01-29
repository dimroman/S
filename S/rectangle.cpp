#include "rectangle.h"
#include "graphics.h"

extern unsigned g_index;

void rectangle::initialize(
	graphics* const owner,
	math::float2 const position,
	float const width,
	float const height
)
{
	static unsigned index = g_index++;

	math::float2 rectangle_vertices[]{
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f }
	};

	unsigned rectangle_indices[]{
		0, 1, 2, 2, 1, 3
	};

	render_object::initialize(
		owner,
		owner->pipeline_state(pipeline_states::triangle_one),
		owner->root_signature(root_signatures::one),
		owner->vertex_buffer_view(rectangle_vertices, sizeof(rectangle_vertices), sizeof(math::float2), index),
		owner->index_buffer_view(rectangle_indices, sizeof(rectangle_indices), DXGI_FORMAT_R32_UINT, index),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	m_position = position;
	m_width = width;
	m_height = height;
}

bool rectangle::update(per_object_constants& object_constants)
{
	if (!render_object::need_to_be_updated())
		return false;

	object_constants.color = { 0.9f, 0.8f, 0.7f, 1.0f };
	if (m_is_selected)
		object_constants.color += { 0.1f, 0.2f, 0.3f, 0.0f };
	if (m_is_highlighted)
		object_constants.color -= { 0.5f, 0.5f, 0.5f, 0.0f };

	object_constants.position = m_position;
	object_constants.width = m_width;
	object_constants.height = m_height;

	return true;
}