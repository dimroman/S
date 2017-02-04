#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "global_defines.h"
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class render_object_owner;

enum
{
	render_objects_count = 1024,
	render_object_instances_count = 1024,
	max_vertex_buffer_views_count = 32,
	max_index_buffer_views_count = 32,
	max_pipeline_states_count = 32,
	max_root_signatures_count = 32,
	max_vertex_shaders_count = 32,
	max_pixel_shaders_count = 32,
};

struct color_constants
{
	math::float4 colors[render_object_instances_count];
};

struct model_view_projection_constants
{
	math::float4x4 model_view_projections[render_object_instances_count];
};

class constant_buffer_data
{
public:
	constant_buffer_data() = default;
	constant_buffer_data(void* const mapped_data, D3D12_GPU_DESCRIPTOR_HANDLE const gpu_handle) :
		m_mapped_data(static_cast<char*>(mapped_data)),
		gpu_handle(gpu_handle)
	{}
	void update(unsigned const offset, void const* const data, unsigned const data_size)
	{
		memcpy(m_mapped_data + offset, data, data_size);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle{ 0 };
private:
	char* m_mapped_data = nullptr;
};

class render_object_instance;

class render_object
{
public:
	void initialize(
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
	);

	void draw(ID3D12GraphicsCommandList* const command_list);
	
	void set_selected(render_object_instance* const instance, bool const value);
	void set_highlighted(render_object_instance* const instance, bool const value);

	void update_model(math::float4x4 const& model_transform, unsigned const instance_id);
	void update_color(math::float4 const& color, unsigned const instance_id);

	inline unsigned instances_count() const { return m_instances_count; }
				
private:
	//ComPtr<ID3D12GraphicsCommandList>	m_bundle;

	ID3D12PipelineState*			m_pipeline_state = nullptr;
	ID3D12RootSignature*			m_root_signature = nullptr;

	enum { max_vertex_buffers = 2, };
	D3D12_VERTEX_BUFFER_VIEW const* m_vertex_buffer_view = nullptr;
	D3D12_INDEX_BUFFER_VIEW const*	m_index_buffer_view = nullptr;	

	D3D12_VERTEX_BUFFER_VIEW const* m_instance_vertex_buffer_view = nullptr;
	render_object_instance*			m_instances = nullptr;
	unsigned						m_instances_count = 0;

	render_object_owner*			m_owner = nullptr;
	D3D_PRIMITIVE_TOPOLOGY			m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED