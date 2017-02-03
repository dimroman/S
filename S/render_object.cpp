#include "render_object.h"
#include "render_object_owner.h"

void render_object::initialize(
	render_object_owner* const owner,
	ID3D12PipelineState* const pipeline_state, 
	ID3D12RootSignature* const root_signature, 
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
	math::float4x4 const& model_transform,
	math::float4 const& color
)
{	
	m_owner = owner;
	m_pipeline_state = pipeline_state;
	m_root_signature = root_signature;
	m_vertex_buffer_view = vertex_buffer_view;
	m_index_buffer_view = index_buffer_view;
	m_primitive_topology = primitive_topology;
	m_model_transform = model_transform;
	m_color = color;
}

bool render_object::need_to_update_model(math::float4x4& model_transform)
{
	//if (!m_model_transform_was_updated)
	//	return false;

	model_transform = m_model_transform;
	//m_model_transform_was_updated--;
	return true;
}

bool render_object::need_to_update_color(math::float4& color)
{
	if (!m_color_was_updated)
		return false;

	color = m_color;
	m_color_was_updated--;
	return true;
}

void render_object::update_model(math::float4x4& model_transform)
{
	m_model_transform = model_transform;
	m_model_transform_was_updated = frames_count;
}

void render_object::update_color(math::float4& color)
{
	m_color = color;
	m_color_was_updated = frames_count;
}

void render_object::set_selected(bool const value) 
{ 
	m_owner->set_selected(value);
}

void render_object::set_highlighted(bool const value) 
{ 
	m_owner->set_highlighted(value);
}