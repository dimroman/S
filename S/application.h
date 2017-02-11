#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "options.h"
#include "graphics.h"
#include "logic.h"
#include "camera.h"
#include "input.h"
#include "math.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class application
{
public:
	void run();

	void select_object	(math::rectangle<math::uint2> const selection);
	void highlight_object(math::rectangle<math::uint2> const selection);
	void remove_all_highlighting();
	void remove_all_selection();

	void move_camera_along_axis_x(float const value);
	void move_camera_along_axis_z(float const value);

	void rotate_camera_around_axis_x(float const value);
	void rotate_camera_around_up_direction(float const value);

	inline void need_to_quit() { m_quit = true; }

private:
	input				m_input;
	logic				m_logic;
	camera				m_camera;
	graphics			m_graphics;
	options				m_options;

	bool				m_quit = false;
};


#endif // #ifndef APPLICATION_H_INCLUDED