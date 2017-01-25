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
	m_static_constants_buffer.preinitialize(owner);

	for ( auto& buffer : m_per_frame_constants)
		buffer.preinitialize(owner);
}

void square_grid::initialize(graphics* const owner, ID3D12Device* const device, ID3D12GraphicsCommandList* const command_list, ID3D12RootSignature* const root_signature, ID3D12PipelineState* pipeline_state, vertex_buffer* const vertex_buffer, float const cell_side_length)
{
	render_object::initialize(pipeline_state, root_signature, vertex_buffer);
		
	m_static_constants_buffer.initialize(device, sizeof(static_constants));

	for ( auto& buffer : m_per_frame_constants )
		buffer.initialize(device, sizeof(per_frame_constants));

	m_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	
	owner->add_render_object(this);

	static_constants constants;
	constants.position = { 0.0f, 0.0f };
	constants.object_width = cell_side_length;
	constants.object_height = cell_side_length;
	constants.id = m_id;
	m_static_constants_buffer.update(&constants, sizeof(static_constants));
}

void square_grid::update(int const current_frame_index, D3D12_GPU_DESCRIPTOR_HANDLE const& per_frame_constants_handle)
{
	m_descriptor_tables[per_frame_constants_descriptor] = per_frame_constants_handle;
	m_descriptor_tables[per_object_per_frame_constants_descriptor] = m_per_frame_constants[current_frame_index].gpu_handle();
	m_descriptor_tables[per_object_static_constants_descriptor] = m_static_constants_buffer.gpu_handle();

	if (!m_updated)
		return;

	per_frame_constants constants;
	constants.color = math::float4(0.1f, 0.2f, 0.3f, 1.0f);

	for ( auto& buffer : m_per_frame_constants )
		buffer.update(&constants, sizeof(per_frame_constants));

	m_updated = false;
}