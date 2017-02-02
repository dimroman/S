#include "render_object.h"
#include "render_object_owner.h"

void render_object::initialize(
	render_object_owner* const owner,
	ID3D12PipelineState* const pipeline_state, 
	ID3D12RootSignature* const root_signature, 
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology
)
{	
	m_owner = owner;
	m_pipeline_state = pipeline_state;
	m_root_signature = root_signature;
	m_vertex_buffer_view = vertex_buffer_view;
	m_index_buffer_view = index_buffer_view;
	m_primitive_topology = primitive_topology;
}

bool render_object::update(per_object_constants& object_constants) 
{ 
	return m_owner->update(object_constants);
}

void render_object::set_selected(bool const value) 
{ 
	m_owner->set_selected(value);
}

void render_object::set_highlighted(bool const value) 
{ 
	m_owner->set_highlighted(value);
}