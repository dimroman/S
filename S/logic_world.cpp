#include "logic_world.h"
#include <assert.h>
#include "logic_object.h"

void logic_world::add_logic_object(logic_object* const object)
{
	m_logic_objects[m_logic_objects_count++] = object;
	assert(m_logic_objects_count < logic_objects_count);
}

void logic_world::update()
{

}