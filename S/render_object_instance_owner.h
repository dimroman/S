#ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED
#define RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED

class render_object_instance_owner
{
public:
	virtual void set_selected(bool const value) { }
	virtual void set_highlighted(bool const value) { }

private:	
};

#endif // #ifndef RENDER_OBJECT_INSTANCE_OWNER_H_INCLUDED