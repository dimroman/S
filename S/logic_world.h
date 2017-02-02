#ifndef LOGIC_WORLD_H_INCLUDED
#define LOGIC_WORLD_H_INCLUDED

#include <utility>

class logic_object;

enum {
	logic_objects_count = 1024 + 512,
};

class logic_world
{
public:
	virtual void update();
	void add_logic_object(logic_object* const object);
protected:

private:
	logic_object*	m_logic_objects[logic_objects_count];
	unsigned int	m_logic_objects_count = 0;
};

#endif // #ifndef LOGIC_WORLD_H_INCLUDED