#include "hexagon_field.h"
#include "graphics.h"

void hexagon_field::initialize(graphics* const graphics)
{
	for (int i = 0; i < field_width; i++)
	{
		float const base_position_x = 2 * m_cell_radii*0.866025f*i - m_cell_radii*0.866025f*(field_width - 1);
		for (int j = 0; j < field_height; j++)
		{
			float const position_x = base_position_x + ((j % 2 == 0) ? 0.0f : m_cell_radii*0.866025f);
			float const position_y = 2 * m_cell_radii *j - 0.866025f * m_cell_radii * (field_height - 1 ) - m_cell_radii*0.5f*j;
			int const index = i + j * field_width;
			m_grid_cells[index].initialize(
				graphics,
				this,
				{ position_x, position_y },
				m_cell_radii
			);
			m_grid_frames[index].initialize(
				graphics,
				{ position_x, position_y },
				m_cell_radii
			);
		}
	}
}

void hexagon_field::update()
{
	super::update();
}