#ifndef HEXAGON_FRAME_H_INCLUDED
#define HEXAGON_FRAME_H_INCLUDED

#include "render_object.h"

class graphics;

class hexagon_frame : public render_object
{
	using super = render_object;
public:
	void initialize(
		graphics* const owner,
		math::float2 const position,
		float const radii
	);
	virtual bool update(per_object_constants& object_constants) override { return false; }
private:
};

#endif // #ifndef HEXAGON_FRAME_H_INCLUDED