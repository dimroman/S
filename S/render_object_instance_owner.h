#ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED
#define RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED

class graphics;

class render_object_instance_owner
{
public:
	void initialize(graphics* const graphics, unsigned const render_object_instance_id);

	virtual void set_selected(bool const value) { }
	virtual void set_highlighted(bool const value) { }

protected:
	unsigned m_render_object_instance_id = unsigned(-1);
	graphics* m_graphics = nullptr;
};

#endif // #ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED