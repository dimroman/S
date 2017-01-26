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

class graphics;

struct constant_buffer_data
{
	constant_buffer_data() = default;
	constant_buffer_data(void* const mapped_data, D3D12_GPU_DESCRIPTOR_HANDLE const gpu_handle) :
		mapped_data(mapped_data),
		gpu_handle(gpu_handle)
	{}
	void* mapped_data = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle{ 0 };
};

class render_object
{
public:
	void initialize(graphics* const owner, ID3D12PipelineState* const pipeline_state, ID3D12RootSignature* const root_signature, D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view, D3D12_INDEX_BUFFER_VIEW const* index_buffer_view, D3D_PRIMITIVE_TOPOLOGY primitive_topology);
	
	inline void set_id			(unsigned const id) { m_id = id; }
	inline void set_selected	(bool const value)	{ m_is_selected = value; m_updated = true; }
	inline void set_highlighted	(bool const value)	{ m_is_highlighted = value; m_updated = true; }

	inline ID3D12PipelineState*					pipeline_state()		const { return m_pipeline_state; }
	inline ID3D12RootSignature*					root_signature()		const { return m_root_signature; }
	inline D3D12_GPU_DESCRIPTOR_HANDLE const*	descriptor_tables()		const { return m_descriptor_tables; }
	inline D3D_PRIMITIVE_TOPOLOGY				primitive_topology()	const { return m_primitive_topology; }
	inline D3D12_VERTEX_BUFFER_VIEW const*		vertex_buffer_view()	const { return m_vertex_buffer_view; }
	inline D3D12_INDEX_BUFFER_VIEW const*		index_buffer_view()		const { return m_index_buffer_view; }
protected:
	unsigned int m_id = 0;

	bool m_is_selected = false;
	bool m_is_highlighted = false;
	bool m_updated = false;

	ID3D12PipelineState* m_pipeline_state = nullptr;
	ID3D12RootSignature* m_root_signature = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE m_descriptor_tables[descriptor_tables_count];
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

	D3D12_VERTEX_BUFFER_VIEW const* m_vertex_buffer_view;
	D3D12_INDEX_BUFFER_VIEW const* m_index_buffer_view;
	unsigned m_index_or_vertex_count = 0;
};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED