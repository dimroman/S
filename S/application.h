#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "options.h"
#include "graphics.h"
#include "logic.h"
#include "camera.h"
#include "math.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class application
{
public:
	application(HINSTANCE handle_instance);
	application(const application& rhs) = delete;
	application& operator=(const application& rhs) = delete;
	~application();

	int run();

	void select_object	(math::rectangle<math::int2> const selection);
	void highlight_object(math::rectangle<math::int2> const selection);
	void remove_all_highlighting();
	void remove_all_selection();

	void move_camera_along_axis_x(float const value);
	void move_camera_along_axis_z(float const value);

	void rotate_camera_around_axis_x(float const value);
	void rotate_camera_around_up_direction(float const value);

private:
	HINSTANCE m_application_instance_handle = nullptr;
	HWND      m_main_window_handle = nullptr;

	graphics			m_graphics;
	logic				m_logic;
	camera				m_camera;
};


#endif // #ifndef APPLICATION_H_INCLUDED