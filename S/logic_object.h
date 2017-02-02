#ifndef LOGIC_OBJECT_H_INCLUDED
#define LOGIC_OBJECT_H_INCLUDED

#include "render_object.h"

class graphics;
class logic_world;

class logic_object : public render_object
{
	using super = render_object;
public:
	virtual bool update(per_object_constants& object_constants) override;

	virtual void set_selected(bool const value) override;
	virtual void set_highlighted(bool const value) override;

	inline bool is_selected() const { return m_is_selected; }
	inline bool is_highlighted() const { return m_is_highlighted; }

	inline void add_neighbour(logic_object& neighbour) { m_neighbours[m_neighbours_count++] = &neighbour; assert(m_neighbours_count <= max_neighbours_count); }

	void update_selection();
	
protected:
	unsigned selection_mask() const;

	void initialize(
		graphics* const graphics,
		logic_world* const owner,
		ID3D12PipelineState* const pipeline_state,
		ID3D12RootSignature* const root_signature,
		D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
		D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
		D3D_PRIMITIVE_TOPOLOGY primitive_topology,
		per_object_constants const& object_constants
	);
private:


protected:
	enum selection_mask_values
	{
		object_in_default_state = 1 << 0,
		object_is_selected = 1 << 1,
		object_is_highlighted = 1 << 2,
		neighbour_is_selected = 1 << 3,
		neighbour_is_highlighted = 1 << 4,
	};

private:
	bool m_is_selected = false;
	bool m_is_highlighted = false;

	unsigned m_selection_mask[frames_count]{ 0 };

	enum { max_neighbours_count = 8, };
	logic_object* m_neighbours[max_neighbours_count]{ nullptr };
	unsigned m_neighbours_count = 0;

	logic_world* m_owner = nullptr;

	unsigned m_need_to_be_updated = 0;
};

#endif // #ifndef LOGIC_OBJECT_H_INCLUDED