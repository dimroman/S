#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#include "options.h"
#include "graphics.h"
#include "logic.h"
#include "camera.h"

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

	void on_mouse_down(int const x, int const y);
	void on_mouse_move(int const new_x, int const new_y);

private:
	HINSTANCE m_application_instance_handle = nullptr;
	HWND      m_main_window_handle = nullptr;

	graphics			m_graphics;
	logic				m_logic;
	world_camera		m_camera;
};


#endif // #ifndef APPLICATION_H_INCLUDED