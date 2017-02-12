#include "square_field.h"
#include "graphics.h"
#include "storage.h"

square_field::square_field(storage* const storage, graphics* const graphics)
{	
	unsigned const rectangle_root_signature_id = graphics->root_signature_id();
	unsigned const rectangle_vertex_buffer_view_id = graphics->vertex_buffer_view_id(assets::rectangle_vertices);
	unsigned const rectangle_index_buffer_view_id = graphics->index_buffer_view_id(assets::rectangle_indices);
	
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
	
	unsigned const grid_pipeline_state_id = graphics->pipeline_state_id(
		rectangle_root_signature_id, 
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 
		assets::vertex_position_float2_layout,
		L"Shaders//vertex_instance_position_x_mvp.hlsl", 
		L"Shaders//instance_color_id.hlsl"
	);
		
	graphics->new_render_object(
		grid_pipeline_state_id,
		rectangle_root_signature_id,
		rectangle_vertex_buffer_view_id,
		unsigned(-1),
		rectangle_index_buffer_view_id,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		field_width*field_height
	);

	storage->new_object(
		model_transforms,
		colors,
		false,
		true,
		field_width*field_height
	);

	math::float4x4 model_transform = math::float4x4::identity();
	model_transform.m[0][0] = m_cell_side_length/2;
	model_transform.m[1][1] = m_cell_side_length/2;
	model_transform.m[2][3] = -0.0005f;

	math::float4 color = math::float4(0.0f, 0.5f, 0.0f, 1.0f);

	graphics->new_render_object(
		grid_pipeline_state_id,
		rectangle_root_signature_id,
		rectangle_vertex_buffer_view_id,
		unsigned(-1),
		rectangle_index_buffer_view_id,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		1
	);

	storage->new_object(
		&model_transform,
		&color,
		true,
		false,
		1
	);

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

	color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);

	unsigned const square_grid_pipeline_state_id = graphics->pipeline_state_id(
		rectangle_root_signature_id, 
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, 
		assets::vertex_position_float2_layout,
		L"Shaders//vertex_position_x_mvp.hlsl", 
		L"Shaders//color_id.hlsl"
	);

	model_transform = math::float4x4::identity();
	model_transform.m[0][0] = m_cell_side_length;
	model_transform.m[1][1] = m_cell_side_length;
	model_transform.m[2][3] = -0.001f;
		
	graphics->new_render_object(
		square_grid_pipeline_state_id,
		rectangle_root_signature_id,
		graphics->vertex_buffer_view_id(vertices),
		unsigned(-1),
		unsigned(-1),
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		1
	);
	
	storage->new_object(
		&model_transform,
		&color,
		false,
		false,
		1
	);
}