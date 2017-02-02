#include "hexagon.h"
#include "graphics.h"

extern unsigned g_index;

void hexagon::initialize(
	graphics* const graphics,
	logic_world* const owner,
	math::float2 const position,
	float const radii
)
{
	static unsigned index = g_index++;

	math::float2 vertices[]{
		{ 0.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 0.866025f, 0.5f },
		{ 0.866025f, -0.5f },
		{ 0.0f, -1.0f },
		{ -0.866025f, -0.5f },
		{ -0.866025f, 0.5f }
	};

	unsigned indices[]{
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 5, 4,
		0, 6, 5,
		0, 1, 6
	};

	per_object_constants object_constants;

	object_constants.color = { 0.9f, 0.8f, 0.7f, 1.0f };
	object_constants.position = m_position;
	object_constants.width = m_radii;
	object_constants.height = m_radii;

	super::initialize(
		graphics,
		owner,
		graphics->pipeline_state(pipeline_states::triangle_one),
		graphics->root_signature(root_signatures::one),
		graphics->vertex_buffer_view(vertices, sizeof(vertices), sizeof(math::float2), index),
		graphics->index_buffer_view(indices, sizeof(indices), DXGI_FORMAT_R32_UINT, index),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		object_constants
	);

	m_position = position;
	m_radii = radii;
}

bool hexagon::update(per_object_constants& object_constants)
{
	if (!super::update(object_constants))
		return false;

	unsigned const selection_mask = super::selection_mask();

	object_constants.color = { 0.9f, 0.8f, 0.7f, 1.0f };
	if (selection_mask & object_is_selected)
		object_constants.color += { 0.1f, 0.2f, 0.3f, 0.0f };
	if (selection_mask & object_is_highlighted)
		object_constants.color -= { 0.5f, 0.5f, 0.5f, 0.0f };

	object_constants.position = m_position;
	object_constants.width = m_radii;
	object_constants.height = m_radii;

	return true;
}