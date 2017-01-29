#ifndef HEXAGON_H_INCLUDED
#define HEXAGON_H_INCLUDED

#include "render_object.h"

class graphics;

class hexagon : public render_object
{
public:
	void initialize(
		graphics* const owner,
		ID3D12PipelineState* const pipeline_state,
		ID3D12RootSignature* const root_signature,
		D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
		D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
		D3D_PRIMITIVE_TOPOLOGY primitive_topology,
		math::float2 const position,
		float const radii
	);
	virtual bool update(per_object_constants& object_constants) override;
private:
	math::float2 m_position;
	float m_radii;
};

#endif // #ifndef HEXAGON_H_INCLUDED