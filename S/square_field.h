#ifndef SQUARE_FIELD_H_INCLUDED
#define SQUARE_FIELD_H_INCLUDED

#include "logic_object.h"

namespace math {
	struct float4x4;
}

class graphics;

class square_field
{
public:
	void initialize( graphics* const graphics, math::float4x4 const& view_projection);

private:
	enum {
		field_width = 31,
		field_height = 31,
	};
	logic_object m_grid_cells[field_width*field_height];
	render_object_instance_owner m_grid_frame;

	float const m_cell_side_length = 1.0f;
};

#endif // #ifndef SQUARE_FIELD_H_INCLUDED