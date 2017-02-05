#ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED
#define LOGIC_OBJECT_INSTANCE_H_INCLUDED

#include <assert.h>
#include "global_defines.h"
#include "render_object_instance_owner.h"

class render_object_instance;

class logic_object : public render_object_instance_owner
{
public:
	void initialize(render_object_instance* const render_object_instance);
	bool update_selection();

	inline bool is_selected() const { return m_is_selected; }
	inline bool is_highlighted() const { return m_is_highlighted; }

	virtual void set_selected(bool const value) override;
	virtual void set_highlighted(bool const value) override;

	inline void add_neighbour(logic_object& neighbour) { m_neighbours[m_neighbours_count++] = &neighbour; assert(m_neighbours_count <= max_neighbours_count); }
private:
	enum selection_mask_values
	{
		object_in_default_state = 1 << 0,
		object_is_selected = 1 << 1,
		object_is_highlighted = 1 << 2,
		neighbour_is_selected = 1 << 3,
		neighbour_is_highlighted = 1 << 4,
	};

	bool m_is_selected = false;
	bool m_is_highlighted = false;

	enum { max_neighbours_count = 8, };
	logic_object* m_neighbours[max_neighbours_count]{ nullptr };
	unsigned m_neighbours_count = 0;
	
	render_object_instance* m_render_object_instance = nullptr;
};

#endif // #ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED