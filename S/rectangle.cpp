#include "rectangle.h"
#include "graphics.h"

extern unsigned g_index;

void rectangle::initialize(
	graphics* const graphics,
	logic_world* const owner,
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
		0, 1, 2, 
		2, 1, 3
	};

	per_object_constants object_constants;

	object_constants.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	object_constants.position = m_position;
	object_constants.width = m_width;
	object_constants.height = m_height;

	super::initialize(
		graphics,
		owner,
		graphics->pipeline_state(pipeline_states::triangle_one),
		graphics->root_signature(root_signatures::one),
		graphics->vertex_buffer_view(rectangle_vertices, sizeof(rectangle_vertices), sizeof(math::float2), index),
		graphics->index_buffer_view(rectangle_indices, sizeof(rectangle_indices), DXGI_FORMAT_R32_UINT, index),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		object_constants
	);

	m_position = position;
	m_width = width;
	m_height = height;
}

bool rectangle::update(per_object_constants& object_constants)
{
	if (!super::update(object_constants))
		return false;

	unsigned const selection_mask = super::selection_mask();

	object_constants.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (selection_mask & object_is_selected)
		object_constants.color = { 0.0f, 1.0f, 0.0f, 1.0f };
	if (selection_mask & neighbour_is_selected)
		object_constants.color = { 0.0f, 0.5f, 0.0f, 1.0f };
	if (selection_mask & object_is_highlighted)
		object_constants.color = { 0.0f, 0.0f, 1.0f, 1.0f };

	object_constants.position = m_position;
	object_constants.width = m_width;
	object_constants.height = m_height;

	return true;
}