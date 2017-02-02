#ifndef HEXAGON_FRAME_H_INCLUDED
#define HEXAGON_FRAME_H_INCLUDED

#include "render_object_owner.h"
#include "math.h"

class graphics;

class hexagon_frame : public render_object_owner
{
	using super = render_object_owner;
public:
	void initialize(
		graphics* const graphics,
		math::float2 const position,
		float const radii
	);
	virtual bool update(per_object_constants& object_constants) override { return false; }
private:
};

#endif // #ifndef HEXAGON_FRAME_H_INCLUDED