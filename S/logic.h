#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include "square_field.h"
#include "hexagon_field.h"

class graphics;
class world_camera;

class logic
{
public:
	void initialize(graphics& graphics, world_camera& camera);
	void update();
	void finalize();
private:
	//hexagon_field m_field;
	square_field m_field;
};

#endif // #ifndef LOGIC_H_INCLUDED