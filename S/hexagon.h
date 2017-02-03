#ifndef HEXAGON_H_INCLUDED
#define HEXAGON_H_INCLUDED

#include "logic_object.h"
#include "math.h"

class graphics;
class logic_world;

class hexagon : public logic_object
{
	using super = logic_object;
public:
	enum { max_neighbours_count = 8 };
public:
	void initialize(
		graphics* const graphics,
		logic_world* const owner,
		math::float4x4 const& view_projection,
		math::float2 const& position,
		float const radii
	);

	virtual bool update_selection() override;
private:
};

#endif // #ifndef HEXAGON_H_INCLUDED