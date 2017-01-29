#include "square_field.h"

void square_field::initialize( graphics* const owner )
{
	for (int i = 0; i < field_width; i++)
	{
		float const position_x = m_cell_side_length*i - m_cell_side_length*(field_width - 1) / 2;
		for (int j = 0; j < field_height; j++)
		{
			float const position_y = m_cell_side_length*j - m_cell_side_length*(field_height - 1) / 2;
			int const index = i + j * field_width;
			m_grid_cells[index].initialize(
				owner,
				{ position_x, position_y },
				m_cell_side_length,
				m_cell_side_length
			);
		}
	}
	
	m_square_grid.initialize(
		owner,
		field_width,
		field_height,
		m_cell_side_length
	);
}