#include "rectangle.h"
#include "camera.h"
#include "d3dx12.h"
#include "graphics.h"
#include "helper_functions.h"

extern world_camera g_camera;

void rectangle::preinitialize(graphics* const owner)
{
	owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 + _countof(m_per_frame_constants));
}

void rectangle::initialize(graphics* const owner, ID3D12Device* const device, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, math::float2 const position, float const width, float const height)
{
	render_object::initialize(owner, pipeline_state, root_signature, vertex_buffer_view, index_buffer_view, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_static_constants = owner->create_constant_buffer_view(sizeof(static_constants));

	for (auto& buffer : m_per_frame_constants)
		buffer = owner->create_constant_buffer_view(sizeof(per_frame_constants));

	static_constants constants;
	constants.position = position;
	constants.object_width = width;
	constants.object_height = height;
	constants.id = m_id;
	memcpy(m_static_constants.mapped_data, &constants, sizeof(static_constants));
}

void rectangle::update(int const current_frame_index)
{
	m_descriptor_tables[per_object_per_frame_constants_descriptor] = m_per_frame_constants[current_frame_index].gpu_handle;
	m_descriptor_tables[per_object_static_constants_descriptor] = m_static_constants.gpu_handle;

	if (!m_updated)
		return;

	per_frame_constants constants;
	constants.color = math::float4(0.9f, 0.8f, 0.7f, 1.0f);
	if (m_is_selected)
		constants.color += math::float4(0.1f, 0.2f, 0.3f, 0.0f);
	if (m_is_highlighted)
		constants.color -= math::float4(0.5f, 0.5f, 0.5f, 0.0f);

	for (auto& buffer : m_per_frame_constants)
		memcpy(buffer.mapped_data, &constants, sizeof(per_frame_constants));

	m_updated = false;
}