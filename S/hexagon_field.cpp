#include "hexagon_field.h"
#include "graphics.h"

void hexagon_field::initialize(graphics* const graphics, math::float4x4 const& view_projection)
{
	math::float2 hexagon_vertices[]{
		{ 0.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 0.866025f, 0.5f },
		{ 0.866025f, -0.5f },
		{ 0.0f, -1.0f },
		{ -0.866025f, -0.5f },
		{ -0.866025f, 0.5f }
	};

	unsigned hexagon_indices[]{
		0, 2, 1,
		0, 3, 2,
		0, 4, 3,
		0, 5, 4,
		0, 6, 5,
		0, 1, 6
	};

	math::float2 hexagon_frame_vertices[]{
		{ 0.0f, 1.0f },
		{ 0.866025f, 0.5f },
		{ 0.866025f, -0.5f },
		{ 0.0f, -1.0f },
		{ -0.866025f, -0.5f },
		{ -0.866025f, 0.5f }
	};

	unsigned hexagon_frame_indices[]{
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0
	};

	auto* const hexagon_root_signature = graphics->root_signature();
	D3D12_INPUT_ELEMENT_DESC const hexagon_input_element_description[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC const hexagon_input_layout_description{ hexagon_input_element_description, _countof(hexagon_input_element_description) };

	auto* const hexagon_pipeline_state = graphics->pipeline_state(hexagon_root_signature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, hexagon_input_layout_description, L"Shaders//vertex_position_x_mvp.hlsl", L"Shaders//color_id.hlsl");
	auto* const hexagon_frame_pipeline_state = graphics->pipeline_state(hexagon_root_signature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE, hexagon_input_layout_description, L"Shaders//vertex_position_x_mvp.hlsl", L"Shaders//color_id.hlsl");
	auto const* const hexagon_vertex_buffer_view = graphics->vertex_buffer_view(hexagon_vertices, sizeof(hexagon_vertices), sizeof(math::float2));
	auto const* const hexagon_index_buffer_view = graphics->index_buffer_view(hexagon_indices, sizeof(hexagon_indices), DXGI_FORMAT_R32_UINT);
	auto const* const hexagon_frame_vertex_buffer_view = graphics->vertex_buffer_view(hexagon_frame_vertices, sizeof(hexagon_frame_vertices), sizeof(math::float2));
	auto const* const hexagon_frame_index_buffer_view = graphics->index_buffer_view(hexagon_frame_indices, sizeof(hexagon_frame_indices), DXGI_FORMAT_R32_UINT);

	for (int i = 0; i < field_width; i++)
	{
		float const base_position_x = 2 * m_cell_radii*0.866025f*i - m_cell_radii*0.866025f*(field_width - 1);
		for (int j = 0; j < field_height; j++)
		{
			float const position_x = base_position_x + ((j % 2 == 0) ? 0.0f : m_cell_radii*0.866025f);
			float const position_y = 2 * m_cell_radii *j - 0.866025f * m_cell_radii * (field_height - 1 ) - m_cell_radii*0.5f*j;
			int const index = i + j * field_width;
			
			math::float4 const hexagon_color = { 1.0f, 1.0f, 1.0f, 1.0f };
			math::float4x4 model_transform = math::float4x4::identity();
			model_transform.m[0][0] = m_cell_radii;
			model_transform.m[1][1] = m_cell_radii;
			model_transform.m[0][3] = position_x;
			model_transform.m[1][3] = position_y;

			m_logic_object_instances[m_logic_object_instances_count].initialize(&m_grid_cells[index]);

			m_grid_cells[index].initialize(
				this,
				graphics->new_render_object(
					&m_grid_cells[index],
					hexagon_pipeline_state,
					hexagon_root_signature,
					hexagon_vertex_buffer_view,
					nullptr,
					hexagon_index_buffer_view,
					D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
					view_projection,
					&model_transform,
					&hexagon_color,
					1
				),
				&m_logic_object_instances[m_logic_object_instances_count],
				1
			);
			m_logic_object_instances_count++;

			math::float4 const hexagon_frame_color = { 0.1f, 0.2f, 0.3f, 1.0f };

			m_grid_frames[index].initialize(
				graphics->new_render_object(
					&m_grid_frames[index],
					hexagon_frame_pipeline_state,
					hexagon_root_signature,
					hexagon_frame_vertex_buffer_view,
					nullptr,
					hexagon_frame_index_buffer_view,
					D3D_PRIMITIVE_TOPOLOGY_LINELIST,
					view_projection,
					&model_transform,
					&hexagon_frame_color,
					1
				)
			);
		}
	}
}

void hexagon_field::update()
{
	super::update();
}