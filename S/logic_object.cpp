#include "logic_object.h"
#include "graphics.h"

extern unsigned g_current_frame_index;

void logic_object::initialize(
	graphics* const graphics,
	logic_world* const owner,
	ID3D12PipelineState* const pipeline_state,
	ID3D12RootSignature* const root_signature,
	D3D12_VERTEX_BUFFER_VIEW const* vertex_buffer_view,
	D3D12_INDEX_BUFFER_VIEW const* index_buffer_view,
	D3D_PRIMITIVE_TOPOLOGY primitive_topology,
	per_object_constants const& object_constants
)
{
	m_render_object = graphics->new_render_object( this, pipeline_state, root_signature, vertex_buffer_view, index_buffer_view, primitive_topology, object_constants);
	m_owner = owner;
	m_owner->add_logic_object(this);

	m_need_to_be_updated = frames_count;
}

void logic_object::set_selected(bool const value)
{ 
	m_is_selected = value; 
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

void logic_object::set_highlighted(bool const value)
{ 
	m_is_highlighted = value;
	for (unsigned i = 0; i < m_neighbours_count; ++i)
		m_neighbours[i]->update_selection();
	update_selection();
}

unsigned logic_object::selection_mask() const
{
	return m_selection_mask[g_current_frame_index];
}

void logic_object::update_selection()
{
	unsigned current_selection_mask = object_in_default_state;

	if (m_is_selected)
		current_selection_mask |= object_is_selected;

	if (m_is_highlighted)
		current_selection_mask |= object_is_highlighted;

	for (unsigned i = 0; i < m_neighbours_count; ++i)
	{
		if (m_neighbours[i]->is_selected())
			current_selection_mask |= neighbour_is_selected;

		if (m_neighbours[i]->is_highlighted())
			current_selection_mask |= neighbour_is_highlighted;
	}

	for (auto& mask : m_selection_mask)
		mask = current_selection_mask;

	m_need_to_be_updated = frames_count;
}

bool logic_object::update(per_object_constants& object_constants)
{
	return m_need_to_be_updated && m_need_to_be_updated--;
}