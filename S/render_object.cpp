#include "render_object.h"
#include "graphics.h"

void render_object::initialize(
	graphics* const graphics,
	ID3D12PipelineState* const pipeline_state, 
	ID3D12RootSignature* const root_signature, 
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
	per_object_constants const& object_constants
)
{
	unsigned const id = graphics->add_render_object(this);

	m_pipeline_state = pipeline_state;
	m_root_signature = root_signature;
	m_vertex_buffer_view = vertex_buffer_view;
	m_index_buffer_view = index_buffer_view;
	m_primitive_topology = primitive_topology;

	for (unsigned i = 0; i < frames_count; ++i)
		graphics->update_render_object(object_constants, id, i);
}