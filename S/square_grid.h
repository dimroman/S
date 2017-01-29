#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "math.h"
#include "render_object.h"

class graphics;

class square_grid : public render_object
{
public:
	void initialize(
		graphics* const owner,
		unsigned const width,
		unsigned const height,
		float const cell_side_length
	);
	virtual bool update(per_object_constants& object_constants) override;
private:
	float m_cell_side_length;
};

#endif // #ifndef SQUARE_GRID_H_INCLUDED