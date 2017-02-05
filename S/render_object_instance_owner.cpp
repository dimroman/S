#include "render_object_instance_owner.h"
#include "graphics.h"

void render_object_instance_owner::initialize(graphics* const graphics, unsigned const render_object_instance_id)
{
	m_graphics = graphics;
	m_render_object_instance_id = render_object_instance_id;
}
