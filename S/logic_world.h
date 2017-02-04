#ifndef LOGIC_WORLD_H_INCLUDED
#define LOGIC_WORLD_H_INCLUDED

#include <utility>
#include "logic_object.h"
#include "logic_object_instance.h"

class logic_object;

enum {
	logic_objects_count = 1024,
	logic_object_instances_count = 1024,
};

class logic_world
{
public:
	virtual void update();
protected:

	logic_object_instance	m_logic_object_instances[logic_object_instances_count];
	unsigned				m_logic_object_instances_count = 0;

private:
};

#endif // #ifndef LOGIC_WORLD_H_INCLUDED