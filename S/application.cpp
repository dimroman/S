#include "application.h"
#include "camera.h"
#include "input.h"
#include <string>
#include "StepTimer.h"

input g_input;
options g_options;
world_camera g_camera;
StepTimer g_timer;

application::application(HINSTANCE handle_instance)	: 
	m_application_instance_handle(handle_instance)
{

}

application::~application()
{

}

void application::on_mouse_down(int const x, int const y)
{
	m_graphics.select_object(x, y);
}

void application::on_mouse_move(int const new_x, int const new_y)
{
	m_graphics.highlight_object(new_x, new_y);
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT8 key = static_cast<UINT8>(wParam);

	switch (msg)
	{
		case WM_KEYDOWN:
		{
			g_input.on_key_down(key);
			return 0;
		}
		case WM_KEYUP:
		{
			g_input.on_key_up(key);
			return 0;
		}
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			g_input.on_mouse_down(wParam, lParam & 0xffff, lParam >> 16 & 0xffff);
			return 0;
		}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
			g_input.on_mouse_up(wParam, lParam & 0xffff, lParam >> 16 & 0xffff);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			g_input.on_mouse_move(wParam, lParam & 0xffff, lParam >> 16 & 0xffff);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int application::run()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_application_instance_handle;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, g_options.screen_width, g_options.screen_height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	m_main_window_handle = 
		CreateWindow(
			L"MainWnd", 
			L"Application",
			WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT, 
			CW_USEDEFAULT, 
			g_options.screen_width,
			g_options.screen_height,
			0, 
			0, 
			m_application_instance_handle, 
			0
		);
	if (!m_main_window_handle)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_main_window_handle, SW_SHOW);
	UpdateWindow(m_main_window_handle);
	
	if (!m_graphics.initialize(m_main_window_handle))
		return 0;

	g_camera.set_look_position({ 0.0f, 0.0f, 0.0f });
	g_input.initialize(this);
	
	MSG msg = { 0 };
	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_timer.Tick(NULL);
			float const last_frame_time = static_cast<float>(g_timer.GetElapsedSeconds());
			float const frameChange = 2.0f * last_frame_time;

			if (g_input.leftArrowPressed)
				g_camera.move_x(-frameChange);
			if (g_input.rightArrowPressed)
				g_camera.move_x(frameChange);
			if (g_input.forward_arrow_pressed)
				g_camera.move_z(-frameChange);
			if (g_input.backward_arrow_pressed)
				g_camera.move_z(frameChange);
			if (g_input.last_dx != 0.0f)
				g_camera.rotate_around_up(-g_input.last_dx);
			if (g_input.last_dy != 0.0f)
				g_camera.rotate_around_x(g_input.last_dy);

			m_graphics.run(last_frame_time);
			
			static int frames_count = 0;
			static float total_frame_time = 0.0f;

			frames_count++;

			if ((g_timer.GetTotalSeconds() - total_frame_time) >= 1.0f)
			{
				float fps = (float)frames_count;
				float mspf = 1000.0f / fps;

				std::wstring fpsStr = std::to_wstring(fps);
				std::wstring mspfStr = std::to_wstring(mspf);

				std::wstring windowText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr;

				SetWindowText(m_main_window_handle, windowText.c_str());

				frames_count = 0;
				total_frame_time += 1.0f;
			}

			g_input.reset();
		}
	}

	m_graphics.finalize();

	return (int)msg.wParam;
}