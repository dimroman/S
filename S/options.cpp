#include "options.h"
#include "graphics.h"

void options::initialize(graphics* const graphics)
{
	m_graphics = graphics;
}

void options::resize()
{
	//if (m_screen_width == 1920 && m_screen_height == 1080)
	//{
	//	m_screen_width = 1280;
	//	m_screen_height = 720;
	//}
	//else if (m_screen_width = 1280 && m_screen_height == 720)
	//{
	//	m_screen_width = 1920;
	//	m_screen_height = 1080;
	//}

	m_graphics->resize(m_screen_width, m_screen_height);
}