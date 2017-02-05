#include "hexagon_field.h"
#include "graphics.h"

void hexagon_field::initialize(graphics* const graphics, math::float4x4 const& view_projection)
{
	math::float4x4 model_transforms[field_width*field_height];
	math::float4 hexagon_colors[field_width*field_height];
	math::float4 hexagon_frame_colors[field_width*field_height];
	
	for (int i = 0; i < field_width; i++)
	{
		float const base_position_x = 2 * m_cell_radii*0.866025f*i - m_cell_radii*0.866025f*(field_width - 1);
		for (int j = 0; j < field_height; j++)
		{
			float const position_x = base_position_x + ((j % 2 == 0) ? 0.0f : m_cell_radii*0.866025f);
			float const position_y = 2 * m_cell_radii *j - 0.866025f * m_cell_radii * (field_height - 1 ) - m_cell_radii*0.5f*j;
			int const index = i + j * field_width;
			
			hexagon_colors[index] = { 1.0f, 1.0f, 1.0f, 1.0f };
			model_transforms[index] = math::float4x4::identity();
			model_transforms[index].m[0][0] = m_cell_radii;
			model_transforms[index].m[1][1] = m_cell_radii;
			model_transforms[index].m[0][3] = position_x;
			model_transforms[index].m[1][3] = position_y;

			hexagon_frame_colors[index] = { 0.1f, 0.2f, 0.3f, 1.0f };
		}
	}
	
	auto* const hexagon_root_signature = graphics->root_signature();
	auto* const hexagon_pipeline_state = graphics->pipeline_state(
		hexagon_root_signature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		assets::vertex_position_float2_layout,
		graphics->vertex_shader(L"Shaders//vertex_instance_position_x_mvp.hlsl"),
		graphics->pixel_shader(L"Shaders//instance_color_id.hlsl")
	);

	unsigned first_render_object_instance_id = unsigned(-1);

	render_object_instance_owner* render_object_instance_owners[field_width*field_height];
	for (unsigned i = 0; i < field_width*field_height; ++i)
		render_object_instance_owners[i] = &m_grid_cells[i];

	graphics->new_render_object(
		render_object_instance_owners,
		hexagon_pipeline_state,
		hexagon_root_signature,
		graphics->vertex_buffer_view(assets::hexagon_vertices),
		nullptr,
		graphics->index_buffer_view(assets::hexagon_indices),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		view_projection,
		&model_transforms[0],
		&hexagon_colors[0],
		field_width*field_height,
		first_render_object_instance_id
	);

	assert(first_render_object_instance_id != unsigned(-1));

	for (unsigned i = 0; i < field_width*field_height; ++i)
		render_object_instance_owners[i]->initialize(graphics, first_render_object_instance_id++);

	auto* const hexagon_frame_pipeline_state = graphics->pipeline_state(
		hexagon_root_signature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		assets::vertex_position_float2_layout,
		graphics->vertex_shader(L"Shaders//vertex_instance_position_x_mvp.hlsl"),
		graphics->pixel_shader(L"Shaders//instance_color_id.hlsl")
	);
	render_object_instance_owner* frames_owner[field_width*field_height];
	for (unsigned i = 0; i < field_width*field_height; ++i)
		frames_owner[i] = &m_grid_frames[i];

	first_render_object_instance_id = unsigned(-1);

	graphics->new_render_object(
		frames_owner,
		hexagon_frame_pipeline_state,
		hexagon_root_signature,
		graphics->vertex_buffer_view(assets::hexagon_frame_vertices),
		nullptr,
		graphics->index_buffer_view(assets::hexagon_frame_indices),
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		view_projection,
		&model_transforms[0],
		&hexagon_frame_colors[0],
		field_width*field_height,
		first_render_object_instance_id
	);

	assert(first_render_object_instance_id != unsigned(-1));

	for (unsigned i = 0; i < field_width*field_height; ++i)
		frames_owner[i]->initialize(graphics, first_render_object_instance_id++);
}