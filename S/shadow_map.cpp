#include "shadow_map.h"
#include "graphics.h"
#include "helper_functions.h"
#include "camera.h"
#include "options.h"

extern options g_options;

void shadow_map::initialize(ID3D12Device* const device, graphics* const heaps_owner)
{
	D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
		{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },	// 1 frequently changed constant buffer.
	};

	CD3DX12_ROOT_PARAMETER1 root_parameters[1];
	root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_root_signature.GetAddressOf())));

	ComPtr<ID3DBlob> vertex_shader;
	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//shadow_shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr));

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{ &StandardVertexDescription[0], 1 };

	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{ true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL, FALSE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
												{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS },
												{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC shadow_map_pso_desc{};
	shadow_map_pso_desc.InputLayout = inputLayoutDesc;
	shadow_map_pso_desc.pRootSignature = m_root_signature.Get();
	shadow_map_pso_desc.VS = { vertex_shader->GetBufferPointer(), vertex_shader->GetBufferSize() };
	shadow_map_pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	shadow_map_pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	shadow_map_pso_desc.DepthStencilState = depth_stencil_desc;
	shadow_map_pso_desc.SampleMask = UINT_MAX;
	shadow_map_pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	shadow_map_pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	shadow_map_pso_desc.SampleDesc.Count = 1;
	shadow_map_pso_desc.NumRenderTargets = 0;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&shadow_map_pso_desc, IID_PPV_ARGS(&m_pipeline_state)));
		
	for ( int i = 0; i < frames_count; ++i)
	{
		m_textures[i].initialize(device, 0, g_options.screen_width, g_options.screen_height, 1, DXGI_FORMAT_R32_TYPELESS, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN,
			&clear_value(DXGI_FORMAT_D32_FLOAT, D3D12_DEPTH_STENCIL_VALUE{ 1.0f,0 }), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, L"shadow_texture", i);
		m_textures[i].initialize_dsv(device, DXGI_FORMAT_D32_FLOAT, heaps_owner);
		m_textures[i].initialize_cbv_srv_uav(device, DXGI_FORMAT_R32_FLOAT, heaps_owner);
	}

	for (auto& buffer : m_constant_buffers)
	{
		buffer.initialize(device, 1);
		buffer.initialize_cbv(device, heaps_owner);
	}


	for (int i = 0; i < frames_count; ++i)
	{
		m_start_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_textures[i].resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_end_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_textures[i].resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
}

void shadow_map::update(int const current_frame_index, world_camera& light_camera)
{
	shadows_constant_buffer shadow_constants{};
	shadow_constants.model_view_projection = math::transpose(math::transpose( math::scale(0.1f, 0.1f, 0.1f) * light_camera.look_at_right_handed() ) * light_camera.perspective_projection_right_handed());
	m_constant_buffers[current_frame_index].update(shadow_constants, 0);
}

void shadow_map::draw(int const current_frame_index, ID3D12GraphicsCommandList* command_list, std::function<void()> draw_callback)
{
	command_list->ResourceBarrier(1, &m_start_transitions[current_frame_index]);

	command_list->ClearDepthStencilView(m_textures[current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	command_list->SetPipelineState(m_pipeline_state.Get());
	command_list->SetGraphicsRootSignature(m_root_signature.Get());

	command_list->SetGraphicsRootDescriptorTable(0, m_constant_buffers[current_frame_index].gpu_handle());
	command_list->OMSetRenderTargets(0, nullptr, false, &m_textures[current_frame_index].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));

	draw_callback();

	command_list->ResourceBarrier(1, &m_end_transitions[current_frame_index]);
}