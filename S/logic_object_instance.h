#ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED
#define LOGIC_OBJECT_INSTANCE_H_INCLUDED

#include <assert.h>
#include "global_defines.h"

class logic_object;

class logic_object_instance
{
public:
	void initialize(logic_object* const owner);
	bool update_selection();

	inline bool is_selected() const { return m_is_selected; }
	inline bool is_highlighted() const { return m_is_highlighted; }

	void set_selected(bool const value);
	void set_highlighted(bool const value);

	inline void add_neighbour(logic_object_instance& neighbour) { m_neighbours[m_neighbours_count++] = &neighbour; assert(m_neighbours_count <= max_neighbours_count); }
private:
	bool m_is_selected = false;
	bool m_is_highlighted = false;

	enum { max_neighbours_count = 8, };
	logic_object_instance* m_neighbours[max_neighbours_count]{ nullptr };
	unsigned m_neighbours_count = 0;

	unsigned m_selection_mask = 0;

	logic_object* m_owner = nullptr;
};

#endif // #ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED