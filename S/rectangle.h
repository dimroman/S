#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include "logic_object.h"

namespace math {
	struct float4x4;
	struct float2;
}

class graphics;
class logic_world;

class rectangle : public logic_object
{
	using super = logic_object;
public:
	void initialize(
		graphics* const graphics,
		logic_world* const owner,
		math::float4x4 const& view_projection,
		math::float2 const& position,
		float const width,
		float const height
	);
	virtual bool update_selection() override;
private:
};

#endif // #ifndef RECTANGLE_H_INCLUDED