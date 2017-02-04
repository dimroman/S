#include "logic_object.h"
#include "logic_world.h"
#include "global_defines.h"
#include "math.h"
#include "render_object.h"
#include "logic_object_instance.h"

void logic_object::initialize(
	logic_world* const owner,
	render_object* const render_object, 
	logic_object_instance* const instances, 
	unsigned const instances_count
)
{
	super::initialize(render_object);
	m_owner = owner;
	m_instances = instances;
	m_instances_count = instances_count;
}

void logic_object::set_selected(unsigned const instance_id, bool const value)
{ 
	m_instances[instance_id].set_selected(value);
}

void logic_object::set_highlighted(unsigned const instance_id, bool const value)
{ 
	m_instances[instance_id].set_highlighted(value);
}

void logic_object::update_color(logic_object_instance* const instance, math::float4 const& color)
{
	super::update_color(color, static_cast<unsigned>(instance - &m_instances[0]));
}
