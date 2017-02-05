#include "logic_object.h"
#include "math.h"
#include "graphics.h"

void logic_object::set_selected(bool const value)
{
	m_is_selected = value;
	update_selection();
}

void logic_object::set_highlighted(bool const value)
{
	m_is_highlighted = value;
	update_selection();
}

bool logic_object::update_selection()
{
	unsigned current_selection_mask = object_in_default_state;

	if (m_is_selected)
		current_selection_mask |= object_is_selected;

	if (m_is_highlighted)
		current_selection_mask |= object_is_highlighted;
	
	math::float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (current_selection_mask & object_is_highlighted)
		color = { 0.25f, 0.25f, 0.25f, 1.0f };
	else if (current_selection_mask & object_is_selected)
		color = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_graphics->update_color(color, m_render_object_instance_id);

	return true;
}