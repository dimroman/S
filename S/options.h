#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include "math.h"

class graphics;

class options
{
public:
	options(graphics* const graphics);
	void resize();
	inline void update_screen_position(math::uint2 const position) { m_screen_position = position; }

	inline unsigned clamp_screen_position_x(unsigned const x) { return std::max<unsigned>(m_screen_position.x, std::min<unsigned>(x, m_screen_position.x + m_screen_width-1)); }
	inline unsigned clamp_screen_position_y(unsigned const y) { return std::max<unsigned>(m_screen_position.y, std::min<unsigned>(y, m_screen_position.y + m_screen_height-1)); }

	inline unsigned screen_width() const { return m_screen_width; }
	inline unsigned screen_height() const { return m_screen_height; }
	inline math::uint2 screen_position() const { return m_screen_position; }

	float const field_of_view = 60.0f;

	float const far_plane = 25.0f;
	float const near_plane = 1.0f;

	inline float aspect_ratio() const { return static_cast<float>(m_screen_width) / static_cast<float>(m_screen_height); }

private:
	math::uint2 m_screen_position{ 0, 0 };

	unsigned m_screen_width = 1920;
	unsigned m_screen_height = 1080;

	graphics* m_graphics = nullptr;
};

#endif // #ifndef OPTIONS_H_INCLUDED