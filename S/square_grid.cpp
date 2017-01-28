#include "square_grid.h"


void square_grid::initialize(
	graphics* const owner,
	ID3D12PipelineState* const pipeline_state,
	ID3D12RootSignature* const root_signature,
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
	D3D_PRIMITIVE_TOPOLOGY primitive_topology,
	float const cell_side_length
)
{
	render_object::initialize(owner, pipeline_state, root_signature, vertex_buffer_view, index_buffer_view, primitive_topology);
	m_cell_side_length = cell_side_length;
}

bool square_grid::update(per_object_constants& object_constants)
{	
	if (!render_object::need_to_be_updated())
		return false;

	object_constants.color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);
	object_constants.position = { 0.0f, 0.0f };
	object_constants.width = m_cell_side_length;
	object_constants.height = m_cell_side_length;

	return true;
}