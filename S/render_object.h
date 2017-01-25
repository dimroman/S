#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

#include <d3d12.h>

enum 
{ 
	frames_count = 2, 
};

enum
{
	per_frame_constants_descriptor,
	per_object_per_frame_constants_descriptor,
	per_object_static_constants_descriptor,
	descriptor_tables_count,
};

class vertex_buffer;

class render_object
{
public:
	inline void initialize(ID3D12PipelineState* const pipeline_state, ID3D12RootSignature* const root_signature, vertex_buffer* const vertex_buffer) 
	{ 
		m_updated = true;
		m_pipeline_state = pipeline_state;
		m_root_signature = root_signature;
		m_vertex_buffer = vertex_buffer;
	}

	inline void set_id			(unsigned const id) { m_id = id; }
	inline void set_selected	(bool const value)	{ m_is_selected = value; m_updated = true; }
	inline void set_highlighted	(bool const value)	{ m_is_highlighted = value; m_updated = true; }

	inline ID3D12PipelineState* pipeline_state() const { return m_pipeline_state; }
	inline ID3D12RootSignature* root_signature() const { return m_root_signature; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE const* descriptor_tables() const { return &m_descriptor_tables[0]; }
	inline D3D_PRIMITIVE_TOPOLOGY primitive_topology() const { return m_primitive_topology; }
	inline vertex_buffer* get_vertex_buffer() const { return m_vertex_buffer; }
protected:
	unsigned int m_id = 0;

	bool m_is_selected = false;
	bool m_is_highlighted = false;
	bool m_updated = false;

	ID3D12PipelineState* m_pipeline_state = nullptr;
	ID3D12RootSignature* m_root_signature = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE m_descriptor_tables[descriptor_tables_count];
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

	vertex_buffer* m_vertex_buffer = nullptr;
};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED