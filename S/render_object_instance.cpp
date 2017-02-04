#include "render_object_instance.h"
#include "render_object.h"

void render_object_instance::initialize(render_object& owner, math::float4x4 const& model_transform, math::float4 const& color)
{
	m_owner = &owner;
	m_model_transform = model_transform;
	m_color = color;
}

bool render_object_instance::need_to_update_model(math::float4x4& model_transform)
{
	//if (!m_model_transform_was_updated)
	//	return false;

	model_transform = m_model_transform;
	//m_model_transform_was_updated--;
	return true;
}

bool render_object_instance::need_to_update_color(math::float4& color)
{
	if (!m_color_was_updated)
		return false;

	color = m_color;
	m_color_was_updated--;
	return true;
}

void render_object_instance::update_model(math::float4x4 const& model_transform)
{
	m_model_transform = model_transform;
	m_model_transform_was_updated = frames_count;
}

void render_object_instance::update_color(math::float4 const& color)
{
	m_color = color;
	m_color_was_updated = frames_count;
}

void render_object_instance::set_selected(bool const value)
{
	m_owner->set_selected(this, value);
}

void render_object_instance::set_highlighted(bool const value)
{
	m_owner->set_highlighted(this, value);
}