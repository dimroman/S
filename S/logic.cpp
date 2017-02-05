#include "logic.h"
#include "camera.h"

void logic::initialize(graphics& graphics, world_camera& camera)
{
	auto const view = camera.look_at_right_handed();
	auto const projection = camera.perspective_projection_right_handed();
	math::float4x4 view_projection;
	math::multiply(projection, view, view_projection);

	m_field.initialize(&graphics, view_projection);
}

void logic::update()
{
}

void logic::finalize()
{

}