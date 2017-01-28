#include "rectangle.h"

void rectangle::initialize(
	graphics* const owner,
	ID3D12PipelineState* const pipeline_state,
	ID3D12RootSignature* const root_signature,
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
	D3D_PRIMITIVE_TOPOLOGY primitive_topology,
	math::float2 const position,
	float const width,
	float const height
)
{
	render_object::initialize(owner, pipeline_state, root_signature, vertex_buffer_view, index_buffer_view, primitive_topology);

	m_position = position;
	m_width = width;
	m_height = height;
}

bool rectangle::update(per_object_constants& object_constants)
{
	if (!render_object::need_to_be_updated())
		return false;

	object_constants.color = { 0.9f, 0.8f, 0.7f, 1.0f };
	if (m_is_selected)
		object_constants.color += { 0.1f, 0.2f, 0.3f, 0.0f };
	if (m_is_highlighted)
		object_constants.color -= { 0.5f, 0.5f, 0.5f, 0.0f };

	object_constants.position = m_position;
	object_constants.width = m_width;
	object_constants.height = m_height;

	return true;
}