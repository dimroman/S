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

graphics* s_graphics = nullptr;

graphics::graphics()
{
	assert(!s_graphics);
	s_graphics = this;
}

void graphics::finalize()
{
	CloseHandle(m_fence_event);
}

void graphics::update(float const last_frame_time)
{
	float frameChange = 2.0f * last_frame_time;
	
	if (g_input.animate)
	{
		for (int i = 0; i < NumLights; i++)
		{
			float diff = frameChange * pow(-1.0f, i);

			m_lights[i].position = m_lights[i].position * math::float4x4::rotation_around_axis_y(diff);
			m_light_cameras[i].set_position(m_lights[i].position.xyz());

			math::float3 const target_position{ 0.0f, 8.0f, 0.0f };
			m_light_cameras[i].set_look_position(target_position);
			m_lights[i].direction = math::normalize(target_position - m_lights[i].position.xyz());
			m_lights[i].view_projection = math::transpose(m_light_cameras[i].look_at_right_handed()*m_light_cameras[i].perspective_projection_right_handed());
		}
	}

	transform_constants_buffer transform_consts{};
	g_buffer_constants g_buffer_consts{};

	transform_consts.model = math::scale(0.1f, 0.1f, 0.1f);
	math::float4x4 const view = g_camera.look_at_right_handed();
	math::float4x4 const projection = g_camera.perspective_projection_right_handed();
	transform_consts.view_projection = math::transpose(view*projection);

	for (int i = 0; i < NumLights; i++)
	{
		memcpy(&g_buffer_consts.lights[i], &m_lights[i], sizeof(LightState));
	}

	g_buffer_consts.view_inverted = math::inverse(view);
	g_buffer_consts.perspective_values = { 1.0f / projection.m[0][0], 1.0f / projection.m[1][1], projection.m[2][3], projection.m[2][2] };
	
	g_buffer_consts.screen_width = static_cast<float>(g_options.screen_width);
	g_buffer_consts.screen_height = static_cast<float>(g_options.screen_height);
	g_buffer_consts.sampleShadowMap = TRUE;
	g_buffer_consts.ambientColor = { 0.1f, 0.2f, 0.3f, 1.0f };
	
	m_transform_constants_buffers[m_current_frame_index].update(transform_consts, 0);
	m_g_buffer_constants[m_current_frame_index].update(g_buffer_consts, 0);
	
	m_shadow_map.update(m_current_frame_index, m_light_cameras[0]);
}

void graphics::run(float const last_frame_time)
{
	update(last_frame_time);
	auto& command_list = m_command_lists[m_current_frame_index];

	m_direct_command_list_allocators[m_current_frame_index]->Reset();
	command_list->Reset(m_direct_command_list_allocators[m_current_frame_index].Get(), nullptr);

	ID3D12DescriptorHeap* ppHeaps[] = { m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV].Get(), m_descriptor_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER].Get() };
	command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	
	command_list->RSSetViewports(1, &m_screen_viewport);
	command_list->RSSetScissorRects(1, &m_scissor_rectangle);

	auto const draw_callback = [&command_list, this]() {
		command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		command_list->IASetVertexBuffers(0, 1, &m_vertex_buffer.view());
		command_list->IASetIndexBuffer(&m_index_buffer.view());
		command_list->OMSetStencilRef(0);

		for ( auto const& drawArgs : Draws )
			command_list->DrawIndexedInstanced(drawArgs.IndexCount, 1, drawArgs.IndexStart, drawArgs.VertexBase, 0);
	};

	m_shadow_map.draw(m_current_frame_index, command_list.Get(), draw_callback);
	
	command_list->ResourceBarrier(1, &m_start_transitions[m_current_frame_index]);

	float const clear_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	command_list->ClearRenderTargetView(m_swap_chain_buffers[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), clear_color, 0, nullptr);
	m_g_buffer.clear(command_list.Get(), m_current_frame_index);

	command_list->ClearDepthStencilView(m_depth_stencils[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE const render_targets[] = {
		m_swap_chain_buffers[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV),
		m_g_buffer.render_target_resource(m_current_frame_index, 0),
		m_g_buffer.render_target_resource(m_current_frame_index, 1),
		m_g_buffer.render_target_resource(m_current_frame_index, 2),
		m_g_buffer.render_target_resource(m_current_frame_index, 3)
	};
	command_list->OMSetRenderTargets(_countof(render_targets), render_targets, FALSE, &m_depth_stencils[m_current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));

	command_list->SetPipelineState(m_pipeline_state.Get());
	command_list->SetGraphicsRootSignature(m_root_signature.Get());

	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command_list->IASetVertexBuffers(0, 1, &m_vertex_buffer.view());
	command_list->IASetIndexBuffer(&m_index_buffer.view());
	command_list->SetGraphicsRootDescriptorTable(2, m_wrap_sampler.gpu_handle());
	command_list->OMSetStencilRef(0);
	command_list->SetGraphicsRootDescriptorTable(1, m_transform_constants_buffers[m_current_frame_index].gpu_handle());

	for ( auto const& drawArgs : Draws )
	{
		command_list->SetGraphicsRootDescriptorTable(0, m_textures[drawArgs.DiffuseTextureIndex].gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		command_list->DrawIndexedInstanced(drawArgs.IndexCount, 1, drawArgs.IndexStart, drawArgs.VertexBase, 0);
	}

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencils[m_current_frame_index].resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	m_g_buffer.draw(
		m_current_frame_index, 
		command_list.Get(), 
		m_g_buffer_constants[m_current_frame_index].gpu_handle(), 
		m_shadow_map.shadow_resource(m_current_frame_index), 
		m_depth_stencils[m_current_frame_index].gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	);

	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencils[m_current_frame_index].resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	command_list->ResourceBarrier(1, &m_end_transitions[m_current_frame_index]);

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