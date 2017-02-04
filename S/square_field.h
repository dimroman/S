#ifndef SQUARE_FIELD_H_INCLUDED
#define SQUARE_FIELD_H_INCLUDED

#include "logic_world.h"
#include "logic_object.h"

namespace math {
	struct float4x4;
}

class graphics;

class square_field : public logic_world
{
	using super = logic_world;
public:
	void initialize( graphics* const graphics, math::float4x4 const& view_projection);
	virtual void update() override;

private:
	enum {
		field_width = 31,
		field_height = 31,
	};
	logic_object m_grid;
	render_object_owner	m_square_grid;
	float const m_cell_side_length = 1.0f;
};

#endif // #ifndef SQUARE_FIELD_H_INCLUDED