#ifndef SQUARE_FIELD_H_INCLUDED
#define SQUARE_FIELD_H_INCLUDED

#include "rectangle.h"
#include "square_grid.h"

class graphics;

class square_field
{
public:
	void initialize( graphics* const owner );
private:
	enum {
		field_width = 32,
		field_height = 32,
	};
	rectangle	m_grid_cells[field_width*field_height];
	square_grid	m_square_grid;
	float const m_cell_side_length = 1.0f;
};

#endif // #ifndef SQUARE_FIELD_H_INCLUDED