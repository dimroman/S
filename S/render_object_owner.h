#ifndef RENDER_OBJECT_OWNER_H_INCLUDED
#define RENDER_OBJECT_OWNER_H_INCLUDED

struct per_object_constants;
class render_object;

namespace math {
	struct float4;
}

class render_object_owner
{
public:
	void initialize(render_object* const render_object);
	virtual void set_selected(unsigned const instance_id, bool const value) { }
	virtual void set_highlighted(unsigned const instance_id, bool const value) { }
	void update_color(math::float4 const& color, unsigned const instance_id);

private:	
	render_object* m_render_object = nullptr;
};

#endif // #ifndef RENDER_OBJECT_OWNER_H_INCLUDED