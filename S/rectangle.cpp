#include "rectangle.h"
#include "graphics.h"

extern unsigned g_index;

void rectangle::initialize(
	graphics* const graphics,
	logic_world* const owner,
	math::float4x4 const& view_projection,
	math::float2 const& position,
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

	math::float4 const color = { 1.0f, 1.0f, 1.0f, 1.0f };
	math::float4x4 model_transform = math::float4x4::identity();
	model_transform.m[0][0] = width;
	model_transform.m[1][1] = height;
	model_transform.m[0][3] = position.x;
	model_transform.m[1][3] = position.y;

	super::initialize(
		owner,
		graphics->new_render_object(
			this,
			graphics->pipeline_state(pipeline_states::triangle_one),
			graphics->root_signature(root_signatures::one),
			graphics->vertex_buffer_view(rectangle_vertices, sizeof(rectangle_vertices), sizeof(math::float2), index),
			graphics->index_buffer_view(rectangle_indices, sizeof(rectangle_indices), DXGI_FORMAT_R32_UINT, index),
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			model_transform,
			view_projection,
			color
		)
	);
}

bool rectangle::update_selection()
{
	if (!super::update_selection())
		return false;

	unsigned const selection_mask = super::selection_mask();

	math::float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (selection_mask & object_is_selected)
		color = { 0.0f, 0.0f, 0.0f, 1.0f };
	if (selection_mask & neighbour_is_selected)
		color = { 0.5f, 0.5f, 0.5f, 1.0f };
	if (selection_mask & object_is_highlighted)
		color = { 0.25f, 0.25f, 0.25f, 1.0f };
	if (selection_mask & neighbour_is_highlighted)
		color = { 0.75f, 0.75f, 0.75f, 1.0f };

	m_render_object->update_color(color);

	return true;
}