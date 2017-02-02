#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include "math.h"
#include "logic_object.h"

class graphics;
class logic_world;

class rectangle : public logic_object
{
	using super = logic_object;
public:
	enum { max_neighbours_count = 8 };
public:
	void initialize(
		graphics* const graphics,
		logic_world* const owner,
		math::float2 const position,
		float const width,
		float const height
	);
	virtual bool update(per_object_constants& object_constants) override;
private:
	math::float2 m_position;
	float m_width;
	float m_height;
};

#endif // #ifndef RECTANGLE_H_INCLUDED