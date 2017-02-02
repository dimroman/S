#include "logic_object.h"
#include "logic_world.h"

extern unsigned g_current_frame_index;

void logic_object::initialize(
	logic_world* const owner,
	render_object* const render_object
)
{
	m_render_object = render_object;
	m_owner = owner;
	m_owner->add_logic_object(this);

	m_need_to_be_updated = frames_count;
}

void logic_object::set_selected(bool const value)
{ 
	m_is_selected = value; 
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

void logic_object::set_highlighted(bool const value)
{ 
	m_is_highlighted = value;
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

unsigned logic_object::selection_mask() const
{
	return m_selection_mask[g_current_frame_index];
}

void logic_object::update_selection()
{
	unsigned current_selection_mask = object_in_default_state;

	if (m_is_selected)
		current_selection_mask |= object_is_selected;

	if (m_is_highlighted)
		current_selection_mask |= object_is_highlighted;

	for (unsigned i = 0; i < m_neighbours_count; ++i)
	{
		if (m_neighbours[i]->is_selected())
			current_selection_mask |= neighbour_is_selected;

		if (m_neighbours[i]->is_highlighted())
			current_selection_mask |= neighbour_is_highlighted;
	}

	for (auto& mask : m_selection_mask)
		mask = current_selection_mask;

	m_need_to_be_updated = frames_count;
}

bool logic_object::update(per_object_constants& object_constants)
{
	return m_need_to_be_updated && m_need_to_be_updated--;
}