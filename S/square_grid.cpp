#include "square_grid.h"
#include <malloc.h>
#include <algorithm>
#include "graphics.h"
#include "camera.h"
#include "d3dx12.h"
#include "helper_functions.h"

extern world_camera g_camera;

void square_grid::preinitialize(graphics* const owner)
{
	owner->increase_descriptor_heap_size(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1 + _countof(m_per_frame_constants));
}

void square_grid::initialize(graphics* const owner, ID3D12Device* const device, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, float const cell_side_length)
{
	render_object::initialize(owner, pipeline_state, root_signature, vertex_buffer_view, index_buffer_view, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	m_static_constants = owner->create_constant_buffer_view(sizeof(static_constants));

	for (auto& buffer : m_per_frame_constants)
		buffer = owner->create_constant_buffer_view(sizeof(per_frame_constants));

	static_constants constants;
	constants.position = { 0.0f, 0.0f };
	constants.object_width = cell_side_length;
	constants.object_height = cell_side_length;
	constants.id = m_id;
	memcpy(m_static_constants.mapped_data, &constants, sizeof(static_constants));
}

void square_grid::update(int const current_frame_index)
{
	m_descriptor_tables[per_object_per_frame_constants_descriptor] = m_per_frame_constants[current_frame_index].gpu_handle;
	m_descriptor_tables[per_object_static_constants_descriptor] = m_static_constants.gpu_handle;

	if (!m_updated)
		return;

	per_frame_constants constants;
	constants.color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);

	for (auto& buffer : m_per_frame_constants)
		memcpy(buffer.mapped_data, &constants, sizeof(per_frame_constants));

	m_updated = false;
}