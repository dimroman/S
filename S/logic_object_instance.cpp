#include "logic_object_instance.h"
#include "logic_object.h"
#include "math.h"

void logic_object_instance::initialize(logic_object* const owner)
{
	m_owner = owner;
}

void logic_object_instance::set_selected(bool const value)
{
	m_is_selected = value;
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

void logic_object_instance::set_highlighted(bool const value)
{
	m_is_highlighted = value;
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

bool logic_object_instance::update_selection()
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

	m_selection_mask = current_selection_mask;

	math::float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (m_selection_mask & neighbour_is_highlighted)
		color = { 0.75f, 0.75f, 0.75f, 1.0f };
	else if (m_selection_mask & object_is_highlighted)
		color = { 0.25f, 0.25f, 0.25f, 1.0f };
	else if (m_selection_mask & neighbour_is_selected)
		color = { 0.5f, 0.5f, 0.5f, 1.0f };
	else if (m_selection_mask & object_is_selected)
		color = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_owner->update_color(this, color);

	return true;
}