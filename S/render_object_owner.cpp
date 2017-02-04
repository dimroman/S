#include "render_object_owner.h"
#include "render_object.h"

void render_object_owner::initialize(render_object* const render_object)
{
	m_render_object = render_object;
}

void render_object_owner::update_color(math::float4 const& color, unsigned const instance_id) 
{ 
	m_render_object->update_color(color, instance_id); 
}