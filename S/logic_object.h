#ifndef LOGIC_OBJECT_H_INCLUDED
#define LOGIC_OBJECT_H_INCLUDED

#include "render_object_owner.h"
#include "global_defines.h"
#include <assert.h>

class graphics;
class logic_world;

class logic_object : public render_object_owner
{
	using super = render_object_owner;
public:
	virtual void set_selected(bool const value) override;
	virtual void set_highlighted(bool const value) override;

	inline bool is_selected() const { return m_is_selected; }
	inline bool is_highlighted() const { return m_is_highlighted; }

	inline void add_neighbour(logic_object& neighbour) { m_neighbours[m_neighbours_count++] = &neighbour; assert(m_neighbours_count <= max_neighbours_count); }

	virtual bool update_selection();
	
protected:
	unsigned selection_mask() const;

	void initialize(
		logic_world* const owner,
		render_object* const render_object
	);
private:


protected:
	enum selection_mask_values
	{
		object_in_default_state = 1 << 0,
		object_is_selected = 1 << 1,
		object_is_highlighted = 1 << 2,
		neighbour_is_selected = 1 << 3,
		neighbour_is_highlighted = 1 << 4,
	};

private:
	bool m_is_selected = false;
	bool m_is_highlighted = false;

	unsigned m_selection_mask[frames_count]{ 0 };

	enum { max_neighbours_count = 8, };
	logic_object* m_neighbours[max_neighbours_count]{ nullptr };
	unsigned m_neighbours_count = 0;

	logic_world* m_owner = nullptr;
};

#endif // #ifndef LOGIC_OBJECT_H_INCLUDED