#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include <d3d12.h>
#include "math.h"
#include "vertex_buffer.h"
#include "constant_buffer.h"
#include "render_object.h"

class graphics;

class rectangle : public render_object
{
public:
	void preinitialize(graphics* const owner);
	void initialize(graphics* const owner, ID3D12Device* device, ID3D12GraphicsCommandList* command_list, math::float2 const position, float const width, float const height);
	void update(int const current_frame_index);
	void draw(ID3D12GraphicsCommandList* const command_list, int const current_frame_index);
private:
	enum { vertices_count = 6, };

	math::float2	m_vertices[vertices_count];

	vertex_buffer	m_vertex_buffer;

	struct transform_constants
	{
		math::float4x4 model_view_projection;
		unsigned int id;
		unsigned int is_highlighted;
		unsigned int is_selected;
		float float_padding[45];
	};

	constant_buffer m_transform_constants_buffer[frames_count];

	ComPtr<ID3D12RootSignature>			m_root_signature;
	ComPtr<ID3D12PipelineState>			m_pipeline_state;
};

#endif // #ifndef RECTANGLE_H_INCLUDED