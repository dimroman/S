#ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED
#define RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED

class render_object_instance;

class render_object_instance_owner
{
public:
	void initialize(render_object_instance* const render_object_instance);

	virtual void set_selected(bool const value) { }
	virtual void set_highlighted(bool const value) { }

protected:
	render_object_instance* m_render_object_instance = nullptr;
};

#endif // #ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED