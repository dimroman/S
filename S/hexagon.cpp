#include "hexagon.h"
#include "graphics.h"

extern unsigned g_index;

void hexagon::initialize(
	graphics* const graphics,
	logic_world* const owner,
	math::float4x4 const& view_projection,
	math::float2 const& position,
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

	math::float4 const color = { 0.9f, 0.8f, 0.7f, 1.0f };
	math::float4x4 model_transform = math::float4x4::identity();
	model_transform.m[0][0] = radii;
	model_transform.m[1][1] = radii;
	model_transform.m[0][3] = position.x;
	model_transform.m[1][3] = position.y;

	super::initialize(
		owner,
		graphics->new_render_object(
			this,
			graphics->pipeline_state(pipeline_states::triangle_one),
			graphics->root_signature(root_signatures::one),
			graphics->vertex_buffer_view(vertices, sizeof(vertices), sizeof(math::float2), index),
			graphics->index_buffer_view(indices, sizeof(indices), DXGI_FORMAT_R32_UINT, index),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			model_transform,
			view_projection,
			color
		)
	);
}

bool hexagon::update_selection()
{
	if (!super::update_selection())
		return false;

	unsigned const selection_mask = super::selection_mask();
	
	math::float4 color = { 0.9f, 0.8f, 0.7f, 1.0f };
	if (selection_mask & object_is_selected)
		color += { 0.1f, 0.2f, 0.3f, 0.0f };
	if (selection_mask & object_is_highlighted)
		color -= { 0.5f, 0.5f, 0.5f, 0.0f };

	m_render_object->update_color(color);

	return true;
}