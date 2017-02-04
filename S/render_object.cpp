#include "render_object.h"
#include "render_object_owner.h"
#include "helper_functions.h"
#include "render_object_instance.h"

void render_object::initialize(
	render_object_owner* const owner,
	ID3D12Device* const device,
	ID3D12CommandAllocator* const bundle_allocator,
	ID3D12PipelineState* const pipeline_state, 
	ID3D12RootSignature* const root_signature, 
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
	D3D12_VERTEX_BUFFER_VIEW const* instance_vertex_buffer_view,
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
	D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
	render_object_instance* instances,
	unsigned const instances_count
)
{
	m_owner = owner;
	m_pipeline_state = pipeline_state;
	m_root_signature = root_signature;
	m_vertex_buffer_view = vertex_buffer_view;
	m_instance_vertex_buffer_view = instance_vertex_buffer_view;
	m_index_buffer_view = index_buffer_view;
	m_primitive_topology = primitive_topology;
	m_instances = instances;
	m_instances_count = instances_count;

	assert(!m_instance_vertex_buffer_view || instances_count == m_instance_vertex_buffer_view->SizeInBytes / m_instance_vertex_buffer_view->StrideInBytes);

	//ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, bundle_allocator, m_pipeline_state, IID_PPV_ARGS(&m_bundle)));
	//m_bundle->SetPipelineState(m_pipeline_state);
	//m_bundle->SetGraphicsRootSignature(m_root_signature);
	//m_bundle->IASetPrimitiveTopology(m_primitive_topology);
	//m_bundle->IASetVertexBuffers(0, 1, m_vertex_buffer_view);
	//if (m_index_buffer_view)
	//{
	//	m_bundle->IASetIndexBuffer(m_index_buffer_view);
	//	m_bundle->DrawIndexedInstanced(m_index_buffer_view->SizeInBytes / 4, 1, 0, 0, 0);
	//}
	//else
	//	m_bundle->DrawInstanced(m_vertex_buffer_view->SizeInBytes / m_vertex_buffer_view->StrideInBytes, 1, 0, 0);
	//ThrowIfFailed(m_bundle->Close());
}

void render_object::draw(ID3D12GraphicsCommandList* const command_list)
{
	//command_list->ExecuteBundle(m_bundle.Get());

	command_list->SetPipelineState(m_pipeline_state);
	command_list->SetGraphicsRootSignature(m_root_signature);
	command_list->IASetPrimitiveTopology(m_primitive_topology);
	command_list->IASetVertexBuffers(0, 1, m_vertex_buffer_view);
	if (m_instance_vertex_buffer_view)
		command_list->IASetVertexBuffers(1, 1, m_instance_vertex_buffer_view);
	if (m_index_buffer_view)
	{
		command_list->IASetIndexBuffer(m_index_buffer_view);
		assert(m_index_buffer_view->Format == DXGI_FORMAT_R32_UINT);
		UINT const index_size = 4;
		UINT const index_count_per_instance = m_index_buffer_view->SizeInBytes / index_size;
		UINT const start_index_location = 0;
		INT const base_vertex_location = 0;
		UINT const start_instance_location = 0;
		command_list->DrawIndexedInstanced(index_count_per_instance, m_instances_count, start_index_location, base_vertex_location, start_instance_location);
	}
	else
	{
		UINT const vertex_count_per_instance = m_vertex_buffer_view->SizeInBytes / m_vertex_buffer_view->StrideInBytes;
		UINT const start_vertex_location = 0;
		UINT const start_instance_location = 0;
		command_list->DrawInstanced(vertex_count_per_instance, m_instances_count, start_vertex_location, start_instance_location);
	}
}

void render_object::update_model(math::float4x4 const& model_transform, unsigned const instance_id) 
{ 
	m_instances[instance_id].update_model(model_transform); 
}

void render_object::update_color(math::float4 const& color, unsigned const instance_id) 
{ 
	m_instances[instance_id].update_color(color); 
}

void render_object::set_selected(render_object_instance* const instance, bool const value)
{
	m_owner->set_selected(static_cast<unsigned>(instance - m_instances), value);
}

void render_object::set_highlighted(render_object_instance* const instance, bool const value)
{
	m_owner->set_highlighted(static_cast<unsigned>(instance - m_instances), value);
}