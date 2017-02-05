#include "logic_object.h"
#include "math.h"
#include "render_object_instance.h"

void logic_object::initialize(render_object_instance* const render_object_instance)
{
	m_render_object_instance = render_object_instance;
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

bool logic_object::update_selection()
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

	math::float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (current_selection_mask & neighbour_is_highlighted)
		color = { 0.75f, 0.75f, 0.75f, 1.0f };
	else if (current_selection_mask & object_is_highlighted)
		color = { 0.25f, 0.25f, 0.25f, 1.0f };
	else if (current_selection_mask & neighbour_is_selected)
		color = { 0.5f, 0.5f, 0.5f, 1.0f };
	else if (current_selection_mask & object_is_selected)
		color = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_render_object_instance->update_color(color);

	return true;
}