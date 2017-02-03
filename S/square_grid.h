#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "render_object_owner.h"

namespace math {
	struct float4x4;
}

class graphics;

class square_grid : public render_object_owner
{
	using super = render_object_owner;
public:
	void initialize(
		graphics* const graphics,
		math::float4x4 const& view_projection,
		unsigned const width,
		unsigned const height,
		float const cell_side_length
	);
private:
};

#endif // #ifndef SQUARE_GRID_H_INCLUDED