#ifndef RENDER_OBJECT_H_INCLUDED
#define RENDER_OBJECT_H_INCLUDED

enum { frames_count = 3, };

class render_object
{
public:
	inline void set_id			(unsigned const id) { m_id = id; }
	inline void set_selected	(bool const value)	{ m_is_selected = value; }
	inline void set_highlighted	(bool const value)	{ m_is_highlighted = value; }

protected:
	unsigned int m_id = 0;

	bool m_is_selected = false;
	bool m_is_highlighted = false;
};

#endif // #ifndef RENDER_OBJECT_H_INCLUDED