#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "render_object_owner.h"
#include "math.h"

class graphics;

class square_grid : public render_object_owner
{
	using super = render_object_owner;
public:
	void initialize(
		graphics* const graphics,
		unsigned const width,
		unsigned const height,
		float const cell_side_length
	);
	virtual bool update(per_object_constants& object_constants) override { return false; }
private:
};

#endif // #ifndef SQUARE_GRID_H_INCLUDED