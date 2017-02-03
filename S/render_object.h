#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "global_defines.h"

class render_object_owner;

enum
{
	render_objects_count = 1024,
	max_vertex_buffer_views_count = 2048,
	max_index_buffer_views_count = 2048,
};

enum
{
	max_neighbours_count = 8,
};

namespace root_signatures {
	enum {
		one,
		count,
	};
}

namespace pipeline_states {
	enum {
		triangle_one,
		line_one,
		count,
	};
}

struct color_constants
{
	math::float4 colors[render_objects_count];
};

struct model_view_projection_constants
{
	math::float4x4 model_view_projections[render_objects_count];
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

class render_object
{
public:
	void initialize(
		render_object_owner* const owner,
		ID3D12PipelineState* const pipeline_state, 
		ID3D12RootSignature* const root_signature, 
		D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
		D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
		D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
		math::float4x4 const& model_transform,
		math::float4 const& color
	);

	bool need_to_update_model(math::float4x4& model_transform);
	bool need_to_update_color(math::float4& color);
	void update_model(math::float4x4& model_transform);
	void update_color(math::float4& color);
	void set_selected(bool const value);
	void set_highlighted(bool const value);

	inline ID3D12PipelineState*					pipeline_state()		const { return m_pipeline_state; }
	inline ID3D12RootSignature*					root_signature()		const { return m_root_signature; }
	inline D3D_PRIMITIVE_TOPOLOGY				primitive_topology()	const { return m_primitive_topology; }
	inline D3D12_VERTEX_BUFFER_VIEW const*		vertex_buffer_view()	const { return m_vertex_buffer_view; }
	inline D3D12_INDEX_BUFFER_VIEW const*		index_buffer_view()		const { return m_index_buffer_view; }
				
private:
	ID3D12PipelineState* m_pipeline_state = nullptr;
	ID3D12RootSignature* m_root_signature = nullptr;

	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

	D3D12_VERTEX_BUFFER_VIEW const* m_vertex_buffer_view;
	D3D12_INDEX_BUFFER_VIEW const* m_index_buffer_view;	

	render_object_owner* m_owner = nullptr;

	math::float4x4	m_model_transform;
	math::float4	m_color;
	unsigned char mutable m_model_transform_was_updated = frames_count;
	unsigned char mutable m_color_was_updated = frames_count;
};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED