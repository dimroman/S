#ifndef SQUARE_GRID_H_INCLUDED
#define SQUARE_GRID_H_INCLUDED

#include "math.h"
#include "render_object.h"

class graphics;

class square_grid : public render_object
{
public:
	void initialize(
		graphics* const owner,
		ID3D12PipelineState* const pipeline_state,
		ID3D12RootSignature* const root_signature,
		D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
		D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
		D3D_PRIMITIVE_TOPOLOGY primitive_topology,
		float const cell_side_length
	);
	virtual bool update(per_object_constants& object_constants) override;
private:
	float m_cell_side_length;
};

#endif // #ifndef SQUARE_GRID_H_INCLUDED