#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include "square_field.h"
#include "hexagon_field.h"
#include <vector>

namespace math {
	struct float4;
}

class graphics;

class logic
{
public:
	logic(graphics& graphics);

private:
	//hexagon_field m_field;
	square_field m_field;
private:
};

#endif // #ifndef LOGIC_H_INCLUDED