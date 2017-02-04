#ifndef RENDER_OBJECT_INSTANCE_H_INCLUDED
#define RENDER_OBJECT_INSTANCE_H_INCLUDED

#include "math.h"
#include "global_defines.h"

class render_object;

class render_object_instance
{
public:
	void initialize(render_object& owner, math::float4x4 const& model_transform, math::float4 const& color);

	bool need_to_update_model(math::float4x4& model_transform);
	bool need_to_update_color(math::float4& color);
	void update_model(math::float4x4 const& model_transform);
	void update_color(math::float4 const& color);

	void set_selected(bool const value);
	void set_highlighted(bool const value);

private:
	math::float4x4	m_model_transform;
	math::float4	m_color;

	unsigned m_model_transform_was_updated = 0;
	unsigned m_color_was_updated = 0;

	render_object* m_owner = nullptr;
};

#endif // #ifndef RENDER_OBJECT_INSTANCE_H_INCLUDED