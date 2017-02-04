#ifndef HEXAGON_FIELD_H_INCLUDED
#define HEXAGON_FIELD_H_INCLUDED

#include "logic_world.h"
#include "logic_object.h"

namespace math {
	struct float4x4;
}

class graphics;

class hexagon_field : public logic_world
{
	using super = logic_world;
public:
	void initialize(graphics* const graphics, math::float4x4 const& view_projection);
	virtual void update() override;
private:
	enum {
		field_width = 16,
		field_height = 16,
	};
	logic_object m_grid_cells[field_width*field_height];
	render_object_owner m_grid_frames[field_width*field_height];

	float const m_cell_radii = 1.0f;
};

#endif // #ifndef HEXAGON_FIELD_H_INCLUDED