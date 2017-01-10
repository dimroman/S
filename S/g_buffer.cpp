#include "g_buffer.h"
#include "graphics.h"
#include "helper_functions.h"

void g_buffer::initialize(ID3D12Device* const device, ID3D12GraphicsCommandList* command_list, graphics* const heaps_owner, DXGI_FORMAT const formats[render_targets_count])
{	
	for ( int i = 0; i < render_targets_count; ++i)
		m_clear_values[i] = clear_value(formats[i], { 0.0f, 0.0f, 0.0f, 1.0f });

	for ( int i = 0; i < frames_count; ++i )
	for( int j = 0; j < render_targets_count; ++j)
	{
		m_textures[i*render_targets_count + j].initialize(device, 0, g_options.screen_width, g_options.screen_height, 1, formats[j], { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, &m_clear_values[j], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, L"render_target", j);
		m_textures[i*render_targets_count + j].initialize_rtv(device, heaps_owner, formats[j]);
		m_textures[i*render_targets_count + j].initialize_cbv_srv_uav(device, formats[j], heaps_owner);
	}

	m_clamp_sampler.initialize(device, heaps_owner, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
	
	D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,		render_targets_count, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,	D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },// render targets
		{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		1, 0, 0,					D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,	D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },// constant buffer
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,		1, render_targets_count, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE,	D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },//shadow map
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV,		1, render_targets_count + 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },//depth buffer
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER,	1, 0, 0,					D3D12_DESCRIPTOR_RANGE_FLAG_NONE,								D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CD3DX12_ROOT_PARAMETER1 root_parameters[5];
	root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);
	root_parameters[4].InitAsDescriptorTable(1, &ranges[4], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_root_signature.GetAddressOf())));

	D3D12_INPUT_ELEMENT_DESC input_element_descs[] =
	{
		{ "ID", 0, DXGI_FORMAT_R32_SINT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> geometry_shader;
	ComPtr<ID3DBlob> pixel_shader;
	UINT const compile_flags{ default_compile_flags() };
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//g_buffer.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//g_buffer.hlsl", nullptr, nullptr, "GSMain", "gs_5_0", compile_flags, 0, &geometry_shader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//g_buffer.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.InputLayout = { input_element_descs, _countof(input_element_descs) };
	pso_desc.pRootSignature = m_root_signature.Get();
	pso_desc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.Get());
	pso_desc.GS = CD3DX12_SHADER_BYTECODE(geometry_shader.Get());
	pso_desc.PS = CD3DX12_SHADER_BYTECODE(pixel_shader.Get());
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState.DepthEnable = FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	pso_desc.NumRenderTargets = 1;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc.Count = 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(m_pipeline_state.GetAddressOf())));

	for (int i = 0; i < frames_count*render_targets_count; ++i)
	{
		m_start_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_textures[i].resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_end_transitions[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_textures[i].resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
}

void g_buffer::clear(ID3D12GraphicsCommandList* command_list, int const current_frame_index)
{
	for ( int i = 0; i < render_targets_count; ++i)
		command_list->ClearRenderTargetView(m_textures[current_frame_index*render_targets_count + i].cpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), m_clear_values[i].Color, 0, nullptr);
}

void g_buffer::draw(int const current_frame_index, ID3D12GraphicsCommandList* command_list, D3D12_GPU_DESCRIPTOR_HANDLE const& scene_constant_buffer, D3D12_GPU_DESCRIPTOR_HANDLE const& shadow_map, D3D12_GPU_DESCRIPTOR_HANDLE const& depth_buffer)
{
	command_list->ResourceBarrier(render_targets_count, &m_start_transitions[current_frame_index*render_targets_count]);

	command_list->SetPipelineState(m_pipeline_state.Get());
	command_list->SetGraphicsRootSignature(m_root_signature.Get());

	command_list->SetGraphicsRootDescriptorTable(0, m_textures[current_frame_index*render_targets_count].gpu_handle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	command_list->SetGraphicsRootDescriptorTable(1, scene_constant_buffer);
	command_list->SetGraphicsRootDescriptorTable(2, shadow_map);
	command_list->SetGraphicsRootDescriptorTable(3, depth_buffer);
	command_list->SetGraphicsRootDescriptorTable(4, m_clamp_sampler.gpu_handle());

	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	command_list->DrawInstanced(1, 1, 0, 0);

	command_list->ResourceBarrier(render_targets_count, &m_end_transitions[current_frame_index*render_targets_count]);
}