#include "rectangle.h"
#include "camera.h"
#include "d3dx12.h"
#include "graphics.h"
#include <d3dcompiler.h>
#include "helper_functions.h"

extern world_camera g_camera;

void rectangle::preinitialize(graphics* const owner)
{
	for ( auto& buffer : m_transform_constants_buffer )
		buffer.preinitialize(owner);
}

void rectangle::initialize(graphics* const owner, ID3D12Device* device, ID3D12GraphicsCommandList* command_list, math::float2 const position, float const width, float const height)
{
	m_vertices[0] = { width / 2 + position.x, height / 2 + position.y };
	m_vertices[1] = { -width / 2 + position.x, height / 2 + position.y };
	m_vertices[2] = { width / 2 + position.x, -height / 2 + position.y };
	m_vertices[3] = { width / 2 + position.x, -height / 2 + position.y };
	m_vertices[4] = { -width / 2 + position.x, height / 2 + position.y };
	m_vertices[5] = { -width / 2 + position.x, -height / 2 + position.y };

	m_vertex_buffer.initialize(device, command_list, (void*)&m_vertices[0], sizeof(math::float2), vertices_count);

	for ( auto& buffer : m_transform_constants_buffer )
		buffer.initialize(device, sizeof(transform_constants));

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{ D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	D3D12_DESCRIPTOR_RANGE1 ranges[] = { // Perfomance TIP: Order from most frequent to least frequent.
		{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV,		1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,						D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	CD3DX12_ROOT_PARAMETER1 root_parameters[_countof(ranges)];
	root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, featureData.HighestVersion, &signature, &error));
	ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_root_signature)));

	ComPtr<ID3DBlob> vertex_shader;
	ComPtr<ID3DBlob> pixel_shader;

	UINT const compile_flags = default_compile_flags();
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//square_vs.hlsl", nullptr, nullptr, "main", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"Shaders//square_ps.hlsl", nullptr, nullptr, "main", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr));

	D3D12_INPUT_ELEMENT_DESC input_element_description[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC input_layout_description{ input_element_description, _countof(input_element_description) };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.InputLayout = input_layout_description;
	pso_desc.pRootSignature = m_root_signature.Get();
	pso_desc.VS = { vertex_shader->GetBufferPointer(), vertex_shader->GetBufferSize() };
	pso_desc.PS = { pixel_shader->GetBufferPointer(), pixel_shader->GetBufferSize() };
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pso_desc.DepthStencilState.DepthEnable = FALSE;
	pso_desc.DepthStencilState.StencilEnable = FALSE;
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso_desc.NumRenderTargets = 2;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.RTVFormats[1] = DXGI_FORMAT_R32_UINT;
	pso_desc.SampleDesc.Count = 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&m_pipeline_state)));

	owner->add_render_object(this);
}

void rectangle::update(int const current_frame_index)
{
	transform_constants constants;
	auto const model = math::float4x4::identity();
	auto const view = g_camera.look_at_right_handed();
	auto const projection = g_camera.perspective_projection_right_handed();
	constants.model_view_projection = math::transpose(model*(view*projection));
	constants.id = m_id;
	constants.is_highlighted = m_is_highlighted ? 1 : 0;
	constants.is_selected = m_is_selected ? 1 : 0;

	m_transform_constants_buffer[current_frame_index].update(&constants, sizeof(transform_constants));
}

void rectangle::draw(ID3D12GraphicsCommandList* command_list, int const current_frame_index)
{
	command_list->SetPipelineState(m_pipeline_state.Get());
	command_list->SetGraphicsRootSignature(m_root_signature.Get());
	command_list->SetGraphicsRootDescriptorTable(0, m_transform_constants_buffer[current_frame_index].gpu_handle());
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command_list->IASetVertexBuffers(0, 1, &m_vertex_buffer.view());
	command_list->DrawInstanced(m_vertex_buffer.vertices_count(), 1, 0, 0);
}