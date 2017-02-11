#include "render_object.h"
#include "helper_functions.h"
#include "graphics.h"

render_object::render_object(
	ID3D12Device* const device,
	ID3D12CommandAllocator* const bundle_allocator,
	unsigned const pipeline_state_id, 
	unsigned const root_signature_id,
	unsigned const vertex_buffer_view_id,
	unsigned const instance_vertex_buffer_view_id,
	unsigned const index_buffer_view_id,
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
	unsigned const instances_count
) :
	m_pipeline_state_id(pipeline_state_id),
	m_root_signature_id(root_signature_id),
	m_vertex_buffer_view_id(vertex_buffer_view_id),
	m_instance_vertex_buffer_view_id(instance_vertex_buffer_view_id),
	m_index_buffer_view_id(index_buffer_view_id),
	m_primitive_topology(primitive_topology),
	m_instances_count(instances_count)
{
}

void render_object::draw(ID3D12GraphicsCommandList* const command_list, graphics* const graphics) const
{
	command_list->SetPipelineState(graphics->pipeline_state(m_pipeline_state_id));
	command_list->SetGraphicsRootSignature(graphics->root_signature(m_root_signature_id));
	command_list->IASetPrimitiveTopology(m_primitive_topology);
	D3D12_VERTEX_BUFFER_VIEW const vertex_buffer_view = graphics->vertex_buffer_view(m_vertex_buffer_view_id);
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
	if (m_instance_vertex_buffer_view_id!=unsigned(-1))
		command_list->IASetVertexBuffers(1, 1, &graphics->vertex_buffer_view(m_instance_vertex_buffer_view_id));
	if (m_index_buffer_view_id!=unsigned(-1))
	{
		D3D12_INDEX_BUFFER_VIEW const index_buffer_view = graphics->index_buffer_view(m_index_buffer_view_id);
		command_list->IASetIndexBuffer(&index_buffer_view);
		assert(index_buffer_view.Format == DXGI_FORMAT_R32_UINT);
		UINT const index_size = 4;
		UINT const index_count_per_instance = index_buffer_view.SizeInBytes / index_size;
		UINT const start_index_location = 0;
		INT const base_vertex_location = 0;
		UINT const start_instance_location = 0;
		command_list->DrawIndexedInstanced(index_count_per_instance, m_instances_count, start_index_location, base_vertex_location, start_instance_location);
	}
	else
	{
		UINT const vertex_count_per_instance = vertex_buffer_view.SizeInBytes / vertex_buffer_view.StrideInBytes;
		UINT const start_vertex_location = 0;
		UINT const start_instance_location = 0;
		command_list->DrawInstanced(vertex_count_per_instance, m_instances_count, start_vertex_location, start_instance_location);
	}
}