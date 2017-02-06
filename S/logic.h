#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include "square_field.h"
#include "hexagon_field.h"

class graphics;
class logic
{
public:
	void initialize(graphics& graphics);
	void update();
	void finalize();
private:
	//hexagon_field m_field;
	square_field m_field;
};

#endif // #ifndef LOGIC_H_INCLUDED