#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "global_defines.h"
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class graphics;

enum
{
	render_object_instances_count = 1024,
};

class render_object
{
public:
	render_object(
		ID3D12Device* const device,
		ID3D12CommandAllocator* const bundle_allocator,
		unsigned const pipeline_state_id, 
		unsigned const root_signature_id,
		unsigned const vertex_buffer_view_id,
		unsigned const instance_vertex_buffer_view_id,
		unsigned const index_buffer_view_id,
		D3D_PRIMITIVE_TOPOLOGY const primitive_topology,
		unsigned const instances_count
	);

	void draw(ID3D12GraphicsCommandList* const command_list, graphics* const graphics) const;
	
	inline unsigned instances_count() const { return m_instances_count; }
				
private:
	unsigned const					m_pipeline_state_id = unsigned(-1);
	unsigned const					m_root_signature_id = unsigned(-1);
	unsigned const					m_vertex_buffer_view_id = unsigned(-1);
	unsigned const					m_index_buffer_view_id = unsigned(-1);	
	unsigned const					m_instance_vertex_buffer_view_id = unsigned(-1);
	unsigned const					m_instances_count = 0;
	D3D_PRIMITIVE_TOPOLOGY const	m_primitive_topology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };

};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED