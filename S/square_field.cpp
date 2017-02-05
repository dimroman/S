#include "square_field.h"
#include "graphics.h"
#include "render_object_instance.h"

void square_field::initialize( graphics* const graphics, math::float4x4 const& view_projection)
{	
	auto* const rectangle_root_signature = graphics->root_signature();
	auto const* const rectangle_vertex_buffer_view = graphics->vertex_buffer_view(assets::rectangle_vertices);
	auto const* const rectangle_index_buffer_view = graphics->index_buffer_view(assets::rectangle_indices);
	
	math::float4x4 model_transforms[field_width*field_height];
	math::float4 colors[field_width*field_height];

	for (int i = 0; i < field_width; i++)
	{
		float const position_x = m_cell_side_length*i - m_cell_side_length*(field_width - 1) / 2;
		for (int j = 0; j < field_height; j++)
		{
			float const position_y = m_cell_side_length*j - m_cell_side_length*(field_height - 1) / 2;
			int const index = i + j * field_width;

			model_transforms[index] = math::float4x4::identity();
			model_transforms[index].m[0][0] = m_cell_side_length;
			model_transforms[index].m[1][1] = m_cell_side_length;
			model_transforms[index].m[0][3] = position_x;
			model_transforms[index].m[1][3] = position_y;

			colors[index] = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
	
	auto* const grid_pipeline_state = graphics->pipeline_state(
		rectangle_root_signature, 
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 
		assets::vertex_position_float2_layout,
		graphics->vertex_shader(L"Shaders//vertex_instance_position_x_mvp.hlsl"), 
		graphics->pixel_shader(L"Shaders//instance_color_id.hlsl")
	);

	render_object_instance* render_object_instances = nullptr;

	render_object_instance_owner* render_object_instance_owners[field_width*field_height];
	for (unsigned i = 0; i < field_width*field_height; ++i)
		render_object_instance_owners[i] = &m_grid_cells[i];
		
	graphics->new_render_object(
		render_object_instance_owners,
		grid_pipeline_state,
		rectangle_root_signature,
		rectangle_vertex_buffer_view,
		nullptr,
		rectangle_index_buffer_view,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		view_projection,
		model_transforms,
		colors,
		field_width*field_height,
		render_object_instances
	);

	assert(render_object_instances);

	for (unsigned i = 0; i < field_width*field_height; ++i)
		render_object_instance_owners[i]->initialize(&render_object_instances[i]);

	unsigned const vertices_count = 2 * (field_width + 1 + field_height + 1);
	math::float2	vertices[vertices_count];
	unsigned		index{ 0 };

	for (unsigned i = 0; i <= field_width; ++i)
	{
		vertices[index++] = { (float)i - field_width / 2.f, -(field_height / 2.f) };
		vertices[index++] = { (float)i - field_width / 2.f, field_height / 2.f };
	}
	for (unsigned i = 0; i <= field_height; ++i)
	{
		vertices[index++] = { -(field_width / 2.f), (float)i - field_height / 2.f };
		vertices[index++] = { field_width / 2.f, (float)i - field_height / 2.f };
	}
	assert(index == vertices_count);

	math::float4 const color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);

	auto* const square_grid_pipeline_state = graphics->pipeline_state(
		rectangle_root_signature, 
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, 
		assets::vertex_position_float2_layout,
		graphics->vertex_shader(L"Shaders//vertex_position_x_mvp.hlsl"), 
		graphics->pixel_shader(L"Shaders//color_id.hlsl")
	);

	math::float4x4 model_transform = math::float4x4::identity();
	model_transform.m[0][0] = m_cell_side_length;
	model_transform.m[1][1] = m_cell_side_length;

	render_object_instance_owner* frame_owner = &m_grid_frame;

	graphics->new_render_object(
		&frame_owner,
		square_grid_pipeline_state,
		rectangle_root_signature,
		graphics->vertex_buffer_view(vertices),
		nullptr,
		nullptr,
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		view_projection,
		&model_transform,
		&color,
		1,
		render_object_instances
	);
}