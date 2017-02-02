#ifndef RENDER_OBJECT_OWNER_H_INCLUDED
#define RENDER_OBJECT_OWNER_H_INCLUDED

#include <d3d12.h>

struct per_object_constants;
class render_object;

class render_object_owner
{
public:
	virtual bool update(per_object_constants& object_constants) { return false; };
	virtual void set_selected(bool const value) { }
	virtual void set_highlighted(bool const value) { }

protected:	
	render_object* m_render_object = nullptr;
};

#endif // #ifndef RENDER_OBJECT_OWNER_H_INCLUDED