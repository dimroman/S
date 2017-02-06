#include "application.h"
#include "input.h"
#include <string>
#include "StepTimer.h"

input g_input;
options g_options;
StepTimer g_timer;

application::application(HINSTANCE handle_instance)	: 
	m_application_instance_handle(handle_instance)
{

}

application::~application()
{

}

void application::select_object(math::rectangle<math::int2> const selection)
{
	m_graphics.select_object(selection);
}

void application::highlight_object(math::rectangle<math::int2> const selection)
{
	m_graphics.highlight_object(selection);
}

void application::remove_all_highlighting()
{
	m_graphics.remove_all_highlighting();
}

void application::remove_all_selection()
{
	m_graphics.remove_all_selection();
}

void application::move_camera_along_axis_x(float const value)
{
	m_camera.move_x(value);
}

void application::move_camera_along_axis_z(float const value)
{
	m_camera.move_z(value);
}

void application::rotate_camera_around_axis_x(float const value)
{
	//m_camera.rotate_around_x(value);
}

void application::rotate_camera_around_up_direction(float const value)
{
	//m_camera.rotate_around_up(value);
}

int application::run()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
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

	m_camera.set_look_position({ 0.0f, 0.0f, 0.0f });
	m_logic.initialize(m_graphics);
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

			g_input.update(last_frame_time);
			m_logic.update();
			m_graphics.run(last_frame_time, m_camera.look_at_right_handed(), m_camera.perspective_projection_right_handed());
			
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
		}
	}

	m_graphics.finalize();
	m_logic.finalize();

	return (int)msg.wParam;
}