#include "square_field.h"
#include "graphics.h"
#include <functional>

void square_field::initialize( graphics* const owner )
{
	math::float2 rectangle_vertices[]{
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f }
	};
	
	unsigned rectangle_indices[]{
		0, 1, 2, 2, 1, 3
	};

	for (int i = 0; i < field_width; i++)
	{
		float const position_x = m_cell_side_length*i - m_cell_side_length*(field_width - 1) / 2;
		for (int j = 0; j < field_height; j++)
		{
			float const position_y = m_cell_side_length*j - m_cell_side_length*(field_height - 1) / 2;
			int const index = i + j * field_width;
			m_grid_cells[index].initialize(
				owner,
				owner->pipeline_state(pipeline_states::triangle_one),
				owner->root_signature(root_signatures::one),
				owner->vertex_buffer_view(rectangle_vertices, sizeof(rectangle_vertices), sizeof(math::float2), reinterpret_cast<unsigned>(this)),
				owner->index_buffer_view(rectangle_indices, sizeof(rectangle_indices), DXGI_FORMAT_R32_UINT, reinterpret_cast<unsigned>(this)),
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				{ position_x, position_y },
				m_cell_side_length,
				m_cell_side_length
			);
		}
	}

	unsigned const grid_vertices_count = 2 * (field_width + 1 + field_height + 1);
	math::float2	grid_vertices[grid_vertices_count];
	unsigned		index{ 0 };

	for (unsigned i = 0; i <= field_width; ++i)
	{
		grid_vertices[index++] = { (float)i - field_width / 2.f, -(field_height / 2.f) };
		grid_vertices[index++] = { (float)i - field_width / 2.f, field_height / 2.f };
	}
	for (unsigned i = 0; i <= field_height; ++i)
	{
		grid_vertices[index++] = { -(field_width / 2.f), (float)i - field_height / 2.f };
		grid_vertices[index++] = { field_width / 2.f, (float)i - field_height / 2.f };
	}
	assert(index == grid_vertices_count);

	m_square_grid.initialize(
		owner,
		owner->pipeline_state(pipeline_states::line_one),
		owner->root_signature(root_signatures::one),
		owner->vertex_buffer_view(reinterpret_cast<BYTE*>(grid_vertices), sizeof(grid_vertices), sizeof(math::float2), reinterpret_cast<unsigned>(this) + 1),
		nullptr,
		D3D_PRIMITIVE_TOPOLOGY_LINELIST, m_cell_side_length
	);
}