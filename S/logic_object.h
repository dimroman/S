#ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED
#define LOGIC_OBJECT_INSTANCE_H_INCLUDED

#include <assert.h>
#include "global_defines.h"
#include "render_object_instance_owner.h"

class logic_object : public render_object_instance_owner
{
public:
	bool update_selection();

	inline bool is_selected() const { return m_is_selected; }
	inline bool is_highlighted() const { return m_is_highlighted; }

	virtual void set_selected(bool const value) override;
	virtual void set_highlighted(bool const value) override;
private:
	enum selection_mask_values
	{
		object_in_default_state = 1 << 0,
		object_is_selected = 1 << 1,
		object_is_highlighted = 1 << 2,
	};

	bool m_is_selected = false;
	bool m_is_highlighted = false;
};

#endif // #ifndef LOGIC_OBJECT_INSTANCE_H_INCLUDED