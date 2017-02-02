#ifndef HEXAGON_FIELD_H_INCLUDED
#define HEXAGON_FIELD_H_INCLUDED

#include "logic_world.h"
#include "hexagon.h"
#include "hexagon_frame.h"

class graphics;

class hexagon_field : public logic_world
{
	using super = logic_world;
public:
	void initialize(graphics* const owner);
	virtual void update() override;
private:
	enum {
		field_width = 16,
		field_height = 16,
	};
	hexagon m_grid_cells[field_width*field_height];
	hexagon_frame m_grid_frames[field_width*field_height];

	float const m_cell_radii = 1.0f;
};

#endif // #ifndef HEXAGON_FIELD_H_INCLUDED