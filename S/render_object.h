#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

#include <d3d12.h>
#include "math.h"

enum 
{ 
	frames_count = 2, 
};

enum 
{ 
	render_objects_count = 1024 + 512, 
};

struct per_object_constants
{
	math::float4 color;
	math::float2 position;
	float width;
	float height;
};

struct per_frame_constants
{
	math::float4x4 model_view_projection;
	per_object_constants object_constants[render_objects_count];
};

class graphics;

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
		graphics* const owner, 
		ID3D12PipelineState* const pipeline_state, 
		ID3D12RootSignature* const root_signature, 
		D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, 
		D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, 
		D3D_PRIMITIVE_TOPOLOGY primitive_topology
	);
	virtual bool update(per_object_constants& object_constants) = 0;
	
	inline void set_selected	(bool const value)	{ m_is_selected = value; m_need_to_be_updated = frames_count; }
	inline void set_highlighted	(bool const value)	{ m_is_highlighted = value; m_need_to_be_updated = frames_count; }

	inline ID3D12PipelineState*					pipeline_state()		const { return m_pipeline_state; }
	inline ID3D12RootSignature*					root_signature()		const { return m_root_signature; }
	inline D3D_PRIMITIVE_TOPOLOGY				primitive_topology()	const { return m_primitive_topology; }
	inline D3D12_VERTEX_BUFFER_VIEW const*		vertex_buffer_view()	const { return m_vertex_buffer_view; }
	inline D3D12_INDEX_BUFFER_VIEW const*		index_buffer_view()		const { return m_index_buffer_view; }

			bool								need_to_be_updated();

protected:
	bool m_is_selected = false;
	bool m_is_highlighted = false;
	
private:
	ID3D12PipelineState* m_pipeline_state = nullptr;
	ID3D12RootSignature* m_root_signature = nullptr;

	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

	D3D12_VERTEX_BUFFER_VIEW const* m_vertex_buffer_view;
	D3D12_INDEX_BUFFER_VIEW const* m_index_buffer_view;	

	unsigned m_need_to_be_updated = 0;
};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED