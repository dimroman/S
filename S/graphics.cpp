#include "graphics.h"
#include "helper_functions.h"
#include "d3dx12.h"
#include "options.h"
#include <vector>
#include <process.h>
#include "input.h"

extern world_camera g_camera;
extern options g_options;
extern input g_input;

graphics* g_graphics = nullptr;

graphics::graphics()
{
	assert(!g_graphics);
	g_graphics = this;

	for (auto& t : m_swap_chain_buffers)
		t.preinitialize(this, false, false, true);

	for (auto& t : m_indices_render_targets)
		t.preinitialize(this, false, false, true);
}

void graphics::finalize()
{
	CloseHandle(m_fence_event);
}

void graphics::update(float const last_frame_time)
{
	per_frame_constants constants;
	auto const model = math::float4x4::identity();
	auto const view = g_camera.look_at_right_handed();
	auto const projection = g_camera.perspective_projection_right_handed();
	constants.model_view_projection = math::transpose(model*(view*projection));
	m_per_frame_constants_buffer[m_current_frame_index].update(&constants, sizeof(per_frame_constants));

	for (auto& cell : m_grid_cells)
		cell.update(m_current_frame_index, m_per_frame_constants_buffer[m_current_frame_index].gpu_handle());
	m_square_grid.update(m_current_frame_index, m_per_frame_constants_buffer[m_current_frame_index].gpu_handle());
}

void graphics::run(float const last_frame_time)
{
	m_frame_id++;

	update(last_frame_time);
	auto& command_list = m_command_lists[m_current_frame_index];

	m_direct_command_list_allocators[m_current_frame_index]->Reset();
	command_list->Reset(m_direct_command_list_allocators[m_current_frame_index].Get(), nullptr);

	ID3D12DescriptorHeap* const ppHeaps[] { 
		m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Get() 
	};
	command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	command_list->RSSetViewports(1, &m_screen_viewport);
	command_list->RSSetScissorRects(1, &m_scissor_rectangle);
		
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[m_current_frame_index].resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	D3D12_CPU_DESCRIPTOR_HANDLE const render_targets[] {
		m_swap_chain_buffers[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
		m_indices_render_targets[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	};
	command_list->OMSetRenderTargets(_countof(render_targets), render_targets, FALSE, nullptr);

	float const clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	command_list->ClearRenderTargetView(m_swap_chain_buffers[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), clear_color, 0, nullptr);
	float const zero_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	command_list->ClearRenderTargetView(m_indices_render_targets[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), zero_color, 0, nullptr);

	ID3D12PipelineState* pipeline_state = nullptr;
	ID3D12RootSignature* root_signature = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE descriptor_tables[descriptor_tables_count]{ 0 };
	D3D_PRIMITIVE_TOPOLOGY primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };
	vertex_buffer* vertex_buffer = nullptr;
	
	for (unsigned i = 0; i < m_render_objects_count; ++i )
	{
		auto const* const object = m_render_objects[i];

		if (object->pipeline_state() != pipeline_state)
		{
			pipeline_state = object->pipeline_state();
			command_list->SetPipelineState(pipeline_state);
		}
		if (object->root_signature() != root_signature)
		{
			root_signature = object->root_signature();
			command_list->SetGraphicsRootSignature(root_signature);
		}
		for (int i = 0; i < descriptor_tables_count; ++i)
		{
			if (object->descriptor_tables()[i].ptr != descriptor_tables[i].ptr)
			{
				descriptor_tables[i] = object->descriptor_tables()[i];
				command_list->SetGraphicsRootDescriptorTable(i, descriptor_tables[i]);
			}
		}
		if (object->primitive_topology() != primitive_topology)
		{
			primitive_topology = object->primitive_topology();
			command_list->IASetPrimitiveTopology(primitive_topology);
		}
		if (object->get_vertex_buffer() != vertex_buffer)
		{
			vertex_buffer = object->get_vertex_buffer();
			command_list->IASetVertexBuffers(0, 1, &vertex_buffer->view());
		}
		command_list->DrawInstanced(vertex_buffer->vertices_count(), 1, 0, 0);
	}

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[m_current_frame_index].resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	m_indices_render_targets[m_current_frame_index].update_readback_resource(command_list.Get());

	command_list->Close();
	ID3D12CommandList* command_lists[] = { command_list.Get() };
	m_command_queue->ExecuteCommandLists(1, command_lists);

	ThrowIfFailed(m_swap_chain->Present(0, 0));

	auto const max_fence_value = m_fence_values[(m_current_frame_index + frames_count - 1) % frames_count];

	ThrowIfFailed(m_command_queue->Signal(m_fence.Get(), max_fence_value));

	m_current_frame_index = m_swap_chain->GetCurrentBackBufferIndex();

	if (m_fence->GetCompletedValue() < m_fence_values[m_current_frame_index])
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fence_values[m_current_frame_index], m_fence_event));
		WaitForSingleObject(m_fence_event, INFINITE);
	}

	m_fence_values[m_current_frame_index] = max_fence_value + 1;
}

void graphics::select_object(int const x, int const y)
{
	auto const selected_object_id = m_indices_render_targets[m_current_frame_index].readback_value(x + y * g_options.screen_width);
	
	if ( m_selected_render_object )
		m_selected_render_object->set_selected(false);
	m_selected_render_object = m_render_objects[selected_object_id];
	m_selected_render_object->set_selected(true);	
}

void graphics::highlight_object(int const x, int const y)
{
	auto const highlighted_object_id = m_indices_render_targets[m_current_frame_index].readback_value(x + y * g_options.screen_width);

	if ( m_highlighted_render_object )
		m_highlighted_render_object->set_highlighted(false);
	m_highlighted_render_object = m_render_objects[highlighted_object_id];
	m_highlighted_render_object->set_highlighted(true);
}

void graphics::add_render_object(render_object* const object) 
{ 
	assert(m_render_objects_count < render_objects_count);
	m_render_objects[m_render_objects_count] = object; 
	object->set_id(m_render_objects_count);
	m_render_objects_count++;
}