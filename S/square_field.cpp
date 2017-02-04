#include "square_field.h"
#include "graphics.h"

void square_field::initialize( graphics* const graphics, math::float4x4 const& view_projection)
{	
	auto* const rectangle_root_signature = graphics->root_signature();
	auto const* const rectangle_vertex_buffer_view = graphics->vertex_buffer_view(assets::rectangle_vertices);
	auto const* const rectangle_index_buffer_view = graphics->index_buffer_view(assets::rectangle_indices);

	logic_object_instance* const grid_cells = m_logic_object_instances;

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

			m_logic_object_instances[m_logic_object_instances_count].initialize(&m_grid);
			m_logic_object_instances_count++;
		}
	}
	unsigned grid_cells_count = static_cast<unsigned>(&m_logic_object_instances[m_logic_object_instances_count] - &grid_cells[0]);
	
	auto* const grid_pipeline_state = graphics->pipeline_state(
		rectangle_root_signature, 
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 
		assets::vertex_position_float2_layout,
		graphics->vertex_shader(L"Shaders//vertex_instance_position_x_mvp.hlsl"), 
		graphics->pixel_shader(L"Shaders//instance_color_id.hlsl")
	);
		
	m_grid.initialize(
		this,
		graphics->new_render_object(
			&m_grid,
			grid_pipeline_state,
			rectangle_root_signature,
			rectangle_vertex_buffer_view,
			nullptr,
			rectangle_index_buffer_view,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			view_projection,
			model_transforms,
			colors,
			grid_cells_count
		),
		&grid_cells[0],
		grid_cells_count
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

	m_square_grid.initialize(
		graphics->new_render_object(
			&m_square_grid,
			square_grid_pipeline_state,
			rectangle_root_signature,
			graphics->vertex_buffer_view(vertices),
			nullptr,
			nullptr,
			D3D_PRIMITIVE_TOPOLOGY_LINELIST,
			view_projection,
			&model_transform,
			&color,
			1
		)
	);

	for (int i = 0; i < field_width; i++)
	for (int j = 0; j < field_height; j++)
	{
		int const index = i + j * field_width;
		for (int k = i - 1; k <= i + 1; ++k)
		{
			if (k < 0)
				continue;

			if (k == field_width)
				break;

			for (int l = j - 1; l <= j + 1; ++l)
			{
				if (l < 0)
					continue;

				if (l == field_height)
					break;

				int const neighbour_index = k + l * field_width;
				if ( neighbour_index > index )
				{
					grid_cells[neighbour_index].add_neighbour(grid_cells[index]);
					grid_cells[index].add_neighbour(grid_cells[neighbour_index]);
				}
			}
		}
	}
}

void square_field::update()
{
	super::update();
}