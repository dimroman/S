#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

#include "square_field.h"
#include "hexagon_field.h"

namespace math {
	struct float4;
}

class graphics;
class storage;

class logic
{
public:
	logic(storage* const storage, graphics& graphics);

private:
	//hexagon_field m_field;
	square_field m_field;
private:
};

#endif // #ifndef LOGIC_H_INCLUDED