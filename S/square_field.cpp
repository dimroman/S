#include "square_field.h"
#include "graphics.h"

void square_field::initialize( graphics* const graphics)
{
	for (int i = 0; i < field_width; i++)
	{
		float const position_x = m_cell_side_length*i - m_cell_side_length*(field_width - 1) / 2;
		for (int j = 0; j < field_height; j++)
		{
			float const position_y = m_cell_side_length*j - m_cell_side_length*(field_height - 1) / 2;
			int const index = i + j * field_width;
			m_grid_cells[index].initialize(
				graphics,
				this,
				{ position_x, position_y },
				m_cell_side_length,
				m_cell_side_length
			);
		}
	}
	
	m_square_grid.initialize(
		graphics,
		field_width,
		field_height,
		m_cell_side_length
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
					m_grid_cells[neighbour_index].add_neighbour(m_grid_cells[index]);
					m_grid_cells[index].add_neighbour(m_grid_cells[neighbour_index]);
				}
			}
		}
	}
}

void square_field::update()
{
	super::update();
}