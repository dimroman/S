#include "storage.h"

void storage::new_object(
	math::float4x4 const* const model_transforms,
	math::float4 const* const colors,
	bool const is_selectable,
	bool const is_cell,
	unsigned const instances_count
)
{
	for (unsigned i = 0; i < instances_count; ++i)
	{
		assert(!is_cell || !is_selectable);
		m_model_transforms[m_render_object_instances_count] = model_transforms[i];
		m_colors[m_render_object_instances_count] = colors[i];
		m_objects.push_back(
			logic_object(
				m_model_transforms[m_render_object_instances_count], 
				m_colors[m_render_object_instances_count], 
				is_selectable, 
				is_cell
			)
		);
		m_render_object_instances_count++;
		assert(m_render_object_instances_count < render_object_instances_count);
	}
}

void storage::update_selection_states(bool const* const selection_states)
{
	for (size_t i = 0, n = m_objects.size(); i < n; ++i)
	{
		if (!m_objects[i].is_cell)
			continue;

		if (!selection_states[i])
			continue;
		
		for (size_t j = 0; j < n; ++j)
		{
			if (!m_objects[j].is_selectable)
				continue;

			if (!m_objects[j].is_selected())
				continue;

			m_objects[j].set_position({ m_model_transforms[i].m[0][3], m_model_transforms[i].m[1][3], m_model_transforms[i].m[2][3] - 0.0005f });
		}		
	}

	for (size_t i = 0, n = m_objects.size(); i < n; ++i)
		m_objects[i].set_selected(selection_states[i]);
}

void storage::update_highlight_states(bool const* const highlight_states)
{
	for (size_t i = 0, n = m_objects.size(); i < n; ++i)
		m_objects[i].set_highlighted(highlight_states[i]);
}