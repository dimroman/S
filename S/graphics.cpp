#include "graphics.h"
#include "helper_functions.h"
#include "d3dx12.h"
#include "options.h"
#include <vector>
#include <process.h>

extern options g_options;
extern unsigned g_current_frame_index;

bool operator!=(D3D12_VERTEX_BUFFER_VIEW const left, D3D12_VERTEX_BUFFER_VIEW const right)
{
	return left.BufferLocation != right.BufferLocation || left.SizeInBytes != right.SizeInBytes || left.StrideInBytes != right.StrideInBytes;
}

void graphics::finalize()
{
	CloseHandle(m_fence_event);
}

void graphics::update_render_object_model_view_projection(math::float4x4 const& model_view_projection, unsigned const id, unsigned const current_frame_index)
{
	m_per_frame_model_view_projections[current_frame_index].update(
		id * sizeof(model_view_projection),
		&model_view_projection,
		sizeof(model_view_projection)
	);
}

void graphics::update_render_object_color(math::float4 const& color, unsigned const id, unsigned const current_frame_index)
{
	m_per_frame_colors[current_frame_index].update(
		id * sizeof(color),
		&color,
		sizeof(color)
	);
}

void graphics::update(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed)
{
	math::float4x4 view_projection;
	math::multiply(perspective_projection_right_handed, look_at_right_handed, view_projection);
	
	math::float4x4 model;
	math::float4x4 model_view_projection;
	math::float4 color;

	for (unsigned i = 0; i < m_render_object_instances_count; ++i)
	{
		m_render_object_instances[i].need_to_update_model(model);
		math::multiply(model, view_projection, model_view_projection);
		update_render_object_model_view_projection(model_view_projection, i, g_current_frame_index);		

		if (m_render_object_instances[i].need_to_update_color(color))
			update_render_object_color(color, i, g_current_frame_index);
	}

}

void graphics::run(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed)
{
	m_frame_id++;

	update(last_frame_time, look_at_right_handed, perspective_projection_right_handed);
	auto& command_list = m_command_lists[g_current_frame_index];

	m_direct_command_list_allocators[g_current_frame_index]->Reset();
	command_list->Reset(m_direct_command_list_allocators[g_current_frame_index].Get(), nullptr);

	ID3D12DescriptorHeap* const ppHeaps[] { 
		m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Get() 
	};
	command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	command_list->RSSetViewports(1, &m_screen_viewport);
	command_list->RSSetScissorRects(1, &m_scissor_rectangle);
		
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[g_current_frame_index].resource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	D3D12_CPU_DESCRIPTOR_HANDLE const render_targets[] {
		m_swap_chain_buffers[g_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
		m_indices_render_targets[g_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	};
	command_list->OMSetRenderTargets(_countof(render_targets), render_targets, FALSE, nullptr);

	float const clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	command_list->ClearRenderTargetView(m_swap_chain_buffers[g_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), clear_color, 0, nullptr);
	float const zero_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	command_list->ClearRenderTargetView(m_indices_render_targets[g_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), zero_color, 0, nullptr);
	
	command_list->SetGraphicsRootSignature(m_root_signatures[0].Get());
	command_list->SetGraphicsRootDescriptorTable(1, m_per_frame_model_view_projections[g_current_frame_index].gpu_handle);
	
	unsigned render_instance_index = 0;
	for (unsigned i = 0; i < m_render_objects_count; ++i)
	{
		command_list->SetGraphicsRoot32BitConstant(0, render_instance_index, 0);
		m_render_objects[i].draw(command_list.Get());
		render_instance_index += m_render_objects[i].instances_count();
	}

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[g_current_frame_index].resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	m_indices_render_targets[g_current_frame_index].update_readback_resource(command_list.Get());

	command_list->Close();
	ID3D12CommandList* command_lists[] = { command_list.Get() };
	m_command_queue->ExecuteCommandLists(1, command_lists);

	ThrowIfFailed(m_swap_chain->Present(0, 0));

	auto const max_fence_value = m_fence_values[(g_current_frame_index + frames_count - 1) % frames_count];

	ThrowIfFailed(m_command_queue->Signal(m_fence.Get(), max_fence_value));

	g_current_frame_index = m_swap_chain->GetCurrentBackBufferIndex();

	if (m_fence->GetCompletedValue() < m_fence_values[g_current_frame_index])
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fence_values[g_current_frame_index], m_fence_event));
		WaitForSingleObject(m_fence_event, INFINITE);
	}

	m_fence_values[g_current_frame_index] = max_fence_value + 1;
}

void graphics::select_object(math::rectangle<math::int2> const selection)
{	
	bool selected_object_instances[render_object_instances_count]{ false };

	void* data;
	D3D12_RANGE const read_range{ 0, 0 };
	ThrowIfFailed(m_indices_render_targets[g_current_frame_index].readback_resource()->Map(0, &read_range, &data));
	unsigned const* const ids = static_cast<unsigned*>(data);
	for ( int y = selection.left_down.y; y <= selection.right_up.y; ++y)
	{
		int const start_index = selection.left_down.x + y * g_options.screen_width;
		int const end_index = selection.right_up.x + y * g_options.screen_width + 1;
		SIZE_T const begin = start_index * sizeof(unsigned);
		SIZE_T const end = end_index * sizeof(unsigned);
		
		for ( int i = start_index, n = end_index; i < n; ++i)
			selected_object_instances[ids[i]] = true;
	}
	m_indices_render_targets[g_current_frame_index].readback_resource()->Unmap(0, nullptr);

	for (unsigned i = 0; i < m_render_object_instances_count; ++i)
	{
		if (selected_object_instances[i] == m_selected_object_instances[i])
			continue;
		m_render_object_instances[i].set_selected(selected_object_instances[i]);
		m_selected_object_instances[i] = selected_object_instances[i];
	}
}

void graphics::highlight_object(math::rectangle<math::int2> const selection)
{
	bool highlighted_object_instances[render_object_instances_count]{ false };
	
	void* data;
	D3D12_RANGE const read_range{ 0, 0 };
	ThrowIfFailed(m_indices_render_targets[g_current_frame_index].readback_resource()->Map(0, &read_range, &data));
	unsigned const* const ids = static_cast<unsigned*>(data);
	for ( int y = selection.left_down.y; y <= selection.right_up.y; ++y)
	{
		int const start_index = selection.left_down.x + y * g_options.screen_width;
		int const end_index = selection.right_up.x + y * g_options.screen_width;
		SIZE_T const begin = start_index * sizeof(unsigned);
		SIZE_T const end = end_index * sizeof(unsigned);

		for (int i = start_index, n = end_index; i < n; ++i)
			highlighted_object_instances[ids[i]] = true;
	}
	m_indices_render_targets[g_current_frame_index].readback_resource()->Unmap(0, nullptr);

	for (unsigned i = 0; i < m_render_object_instances_count; ++i)
	{
		if (highlighted_object_instances[i] == m_highlighted_object_instances[i])
			continue;
		m_render_object_instances[i].set_highlighted(highlighted_object_instances[i]);
		m_highlighted_object_instances[i] = highlighted_object_instances[i];
	}
}

void graphics::remove_all_highlighting()
{
	for (unsigned i = 0; i < m_render_object_instances_count; ++i)
	{
		if (m_highlighted_object_instances[i])
			m_render_object_instances[i].set_highlighted(false);
	}
}

void graphics::remove_all_selection()
{
	for (unsigned i = 0; i < m_render_object_instances_count; ++i)
	{
		if (m_selected_object_instances[i])
			m_render_object_instances[i].set_selected(false);
	}
}