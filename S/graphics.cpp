#include "graphics.h"
#include "helper_functions.h"
#include "d3dx12.h"
#include <process.h>
#include "storage.h"

void graphics::update_model_transforms(unsigned const current_frame_index)
{
	m_per_frame_model_transforms[current_frame_index].update(
		0,
		m_storage->model_transforms(),
		m_storage->model_transforms_size()
	);
}

void graphics::update_colors(unsigned const current_frame_index)
{
	m_per_frame_colors[current_frame_index].update(
		0,
		m_storage->colors(),
		m_storage->colors_size()
	);
}

void graphics::update_view_projection_transform(math::float4x4 const& view_projection_transform, unsigned const current_frame_index)
{
	m_per_frame_colors[current_frame_index].update(
		render_object_instances_count*sizeof(math::float4),
		&view_projection_transform,
		sizeof(view_projection_transform)
	);
}

void graphics::update(float const last_frame_time, math::float4x4 const& look_at_right_handed, math::float4x4 const& perspective_projection_right_handed)
{
	m_frame_id++;

	math::float4x4 view_projection;
	math::multiply(perspective_projection_right_handed, look_at_right_handed, view_projection);
	update_view_projection_transform(view_projection, m_current_frame_index);
	
	update_model_transforms(m_current_frame_index);
	update_colors(m_current_frame_index);
}

void graphics::run(unsigned const screen_width, unsigned const screen_height)
{
	auto& command_list = m_command_lists[m_current_frame_index];

	m_direct_command_list_allocators[m_current_frame_index]->Reset();
	command_list->Reset(m_direct_command_list_allocators[m_current_frame_index].Get(), nullptr);

	ID3D12DescriptorHeap* const ppHeaps[] { 
		m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Get() 
	};
	command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	command_list->RSSetViewports(1, &m_screen_viewport);
	command_list->RSSetScissorRects(1, &m_scissor_rectangle);
		
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[m_current_frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	D3D12_CPU_DESCRIPTOR_HANDLE const render_targets[] {
		m_swap_chain_rtv_cpu_handles[m_current_frame_index],
		m_indices_rtv_cpu_handles[m_current_frame_index]
	};

	float const clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	command_list->ClearRenderTargetView(m_swap_chain_rtv_cpu_handles[m_current_frame_index], clear_color, 0, nullptr);
	float const zero_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	command_list->ClearRenderTargetView(m_indices_rtv_cpu_handles[m_current_frame_index], zero_color, 0, nullptr);
	command_list->ClearDepthStencilView(m_depth_stencil_dsv_cpu_handles[m_current_frame_index], D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	command_list->OMSetRenderTargets(_countof(render_targets), render_targets, FALSE, &m_depth_stencil_dsv_cpu_handles[m_current_frame_index]);
	command_list->SetGraphicsRootSignature(m_root_signatures[0].Get());
	command_list->SetGraphicsRootDescriptorTable(1, m_per_frame_model_transforms[m_current_frame_index].gpu_handle);
	
	unsigned render_instance_index = 0;
	for (auto const& object : m_render_objects)
	{
		command_list->SetGraphicsRoot32BitConstant(0, render_instance_index, 0);
		object.draw(command_list.Get(), this);
		render_instance_index += object.instances_count();
	}

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_swap_chain_buffers[m_current_frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	D3D12_TEXTURE_COPY_LOCATION destination;
	destination.pResource = m_indices_rtv_readback_resource[m_current_frame_index].Get();
	destination.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	destination.PlacedFootprint = { 0, { indices_render_target_format, screen_width, screen_height, 1, screen_width*sizeof(indices_render_target_type) } };

	D3D12_TEXTURE_COPY_LOCATION source;
	source.pResource = m_indices_render_targets[m_current_frame_index].Get();
	source.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	source.SubresourceIndex = 0;

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indices_render_targets[m_current_frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));
	command_list->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indices_render_targets[m_current_frame_index].Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	
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

void graphics::select_object(math::rectangle<math::uint2> const selection, unsigned const screen_width)
{ 
	bool states[render_object_instances_count]{ false };
	update_object_selection_impl(selection, screen_width, states); 
	m_storage->update_selection_states(states);

}
void graphics::highlight_object(math::rectangle<math::uint2> const selection, unsigned const screen_width) 
{
	bool states[render_object_instances_count]{ false };
	update_object_selection_impl(selection, screen_width, states); 
	m_storage->update_highlight_states(states);
}

void graphics::update_object_selection_impl(math::rectangle<math::uint2> const selection, unsigned const screen_width, bool* const current_selection_states)
{
	void* data;
	D3D12_RANGE const read_range{ 0, 0 };
	ThrowIfFailed(m_indices_rtv_readback_resource[m_current_frame_index]->Map(0, &read_range, &data));
	auto const* const ids = static_cast<indices_render_target_type const* const>(data);
	unsigned const step = 10;
	for (unsigned y = selection.lower_bound.y; y <= selection.upper_bound.y; y += step)
	{
		unsigned const start_index = selection.lower_bound.x + y * screen_width;
		unsigned const end_index = selection.upper_bound.x + y * screen_width;

		for (unsigned i = start_index, n = end_index; i <= n; i += step)
		{
			auto const id = ids[i];
			current_selection_states[id] = true;
		}
	}
	m_indices_rtv_readback_resource[m_current_frame_index]->Unmap(0, nullptr);
}

void graphics::remove_all_highlighting()
{
	bool states[render_object_instances_count]{ false };
	m_storage->update_highlight_states(states);
}

void graphics::remove_all_selection()
{
	bool states[render_object_instances_count]{ false };
	m_storage->update_selection_states(states);
}