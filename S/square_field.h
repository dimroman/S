#ifndef SQUARE_FIELD_H_INCLUDED
#define SQUARE_FIELD_H_INCLUDED

namespace math {
	struct float4x4;
}

class graphics;
class storage;

class square_field
{
public:
	square_field(storage* const storage, graphics* const graphics);

private:
	enum {
		field_width = 31,
		field_height = 31,
	};

	float const m_cell_side_length = 1.0f;
};

#endif // #ifndef SQUARE_FIELD_H_INCLUDED