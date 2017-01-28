#include "render_object.h"
#include "graphics.h"

void render_object::initialize(
	graphics* const owner, 
	ID3D12PipelineState* const pipeline_state, 
	ID3D12RootSignature* const root_signature, 
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
	D3D_PRIMITIVE_TOPOLOGY primitive_topology
)
{
	m_need_to_be_updated = frames_count;

	m_pipeline_state = pipeline_state;
	m_root_signature = root_signature;
	m_vertex_buffer_view = vertex_buffer_view;
	m_index_buffer_view = index_buffer_view;
	m_primitive_topology = primitive_topology;

	owner->add_render_object(this);
}

bool render_object::need_to_be_updated()
{
	return m_need_to_be_updated && (--m_need_to_be_updated + 1);
}