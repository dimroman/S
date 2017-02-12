#ifndef HEXAGON_FIELD_H_INCLUDED
#define HEXAGON_FIELD_H_INCLUDED

namespace math {
	struct float4x4;
}

class graphics;
class storage;

class hexagon_field
{
public:
	hexagon_field(storage* const storage, graphics* const graphics);
private:
	enum {
		field_width = 16,
		field_height = 16,
	};

	float const m_cell_radii = 1.0f;
};

#endif // #ifndef HEXAGON_FIELD_H_INCLUDED