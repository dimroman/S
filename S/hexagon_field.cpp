#include "hexagon_field.h"
#include "graphics.h"
#include "storage.h"

hexagon_field::hexagon_field(storage* const storage, graphics* const graphics)
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
	
	unsigned const hexagon_root_signature_id = graphics->root_signature_id();
	unsigned const hexagon_pipeline_state_id = graphics->pipeline_state_id(
		hexagon_root_signature_id,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		assets::vertex_position_float2_layout,
		L"Shaders//vertex_instance_position_x_mvp.hlsl",
		L"Shaders//instance_color_id.hlsl"
	);

	graphics->new_render_object(
		hexagon_pipeline_state_id,
		hexagon_root_signature_id,
		graphics->vertex_buffer_view_id(assets::hexagon_vertices),
		unsigned(-1),
		graphics->index_buffer_view_id(assets::hexagon_indices),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		field_width*field_height
	);

	storage->new_object(
		model_transforms,
		hexagon_colors,
		false,
		true,
		field_width*field_height
	);

	unsigned const hexagon_frame_pipeline_state_id = graphics->pipeline_state_id(
		hexagon_root_signature_id,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		assets::vertex_position_float2_layout,
		L"Shaders//vertex_instance_position_x_mvp.hlsl",
		L"Shaders//instance_color_id.hlsl"
	);
	
	graphics->new_render_object(
		hexagon_frame_pipeline_state_id,
		hexagon_root_signature_id,
		graphics->vertex_buffer_view_id(assets::hexagon_frame_vertices),
		unsigned(-1),
		graphics->index_buffer_view_id(assets::hexagon_frame_indices),
		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
		field_width*field_height
	);
	
	storage->new_object(
		model_transforms,
		hexagon_frame_colors,
		false,
		false,
		field_width*field_height
	);
}