#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include "math.h"
#include "render_object.h"
#include <vector>

class logic_object
{
public:
	logic_object(
		math::float4x4& model_transform, 
		math::float4& color, 
		bool const is_selectable, 
		bool const is_cell
	) : 
		model_transform(model_transform),
		color(color),
		is_selectable(is_selectable),
		is_cell(is_cell)
	{}

	void set_highlighted(bool const value)
	{
		if (!is_selectable)
			return;

		if (value == m_is_highlighted)
			return;

		m_is_highlighted = value;
	}

	void set_selected(bool const value)
	{
		if (!is_selectable)
			return;

		if (value == m_is_selected)
			return;

		m_is_selected = value;
	}

	void set_position(math::float3 const& position)
	{
		model_transform.m[0][3] = position.x;
		model_transform.m[1][3] = position.y;
		model_transform.m[2][3] = position.z;
	}

	inline math::float3		position() const { return{ model_transform.m[0][3], model_transform.m[1][3], model_transform.m[2][3] }; }
	inline bool				is_selected() const { return m_is_selected; }

	math::float4x4& model_transform;
	math::float4& color;

	bool const is_selectable = false;
	bool const is_cell = false;
private:
	bool m_is_selected = false;
	bool m_is_highlighted = false;
};

class storage
{
public:
	void new_object(
		math::float4x4 const* const model_transforms,
		math::float4 const* const colors,
		bool const is_selectable,
		bool const is_cell,
		unsigned const instances_count
	);

	void update_selection_states(bool const* const selection_states);
	void update_highlight_states(bool const* const highlight_states);

	inline void const* model_transforms() const { return static_cast<void const*>(m_model_transforms); }
	inline unsigned model_transforms_size() const { return sizeof(m_model_transforms); }

	inline void const* colors() const { return static_cast<void const*>(m_colors); }
	inline unsigned colors_size() const { return sizeof(m_colors); }

private:
	math::float4x4						m_model_transforms[render_object_instances_count];
	math::float4						m_colors[render_object_instances_count];
	std::vector<logic_object>			m_objects;
	unsigned							m_render_object_instances_count = 0;
};

#endif // #ifndef STORAGE_H_INCLUDED