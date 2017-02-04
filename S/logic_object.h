#ifndef LOGIC_OBJECT_H_INCLUDED
#define LOGIC_OBJECT_H_INCLUDED

#include "render_object_owner.h"
#include "global_defines.h"
#include <assert.h>

class graphics;
class logic_world;

class logic_object_instance;

class logic_object : public render_object_owner
{
	using super = render_object_owner;
public:

	void initialize(logic_world* const owner, render_object* const render_object, logic_object_instance* const instances, unsigned const instances_count);

	virtual void set_selected(unsigned const instance_id, bool const value) override;
	virtual void set_highlighted(unsigned const instance_id, bool const value) override;

	void update_color(logic_object_instance* const instance, math::float4 const& color);

protected:

private:
	logic_world* m_owner = nullptr;
	logic_object_instance* m_instances = nullptr;
	unsigned m_instances_count = 0;
};

#endif // #ifndef LOGIC_OBJECT_H_INCLUDED