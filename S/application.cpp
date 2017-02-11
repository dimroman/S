#include "application.h"
#include <string>
#include "StepTimer.h"

StepTimer g_timer;

void application::select_object(math::rectangle<math::uint2> const selection)
{
	m_graphics.select_object(selection, m_options.screen_width());
}

void application::highlight_object(math::rectangle<math::uint2> const selection)
{
	m_graphics.highlight_object(selection, m_options.screen_width());
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
	//m_camera.move_x(value);
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

void application::run()
{	
	m_options.initialize(&m_graphics);
	m_graphics.initialize(m_options.screen_width(), m_options.screen_height());
	m_camera.set_look_position({ 0.0f, 0.0f, 0.0f });
	m_logic.initialize(m_graphics);
	m_input.initialize(this, &m_options);
		
	MSG msg;

	unsigned frames_count = 0;
	float total_frame_time = 0.0f;
	std::wstring fps_text;
	
	while (!m_quit)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{}
		g_timer.Tick(NULL);
		float const last_frame_time = static_cast<float>(g_timer.GetElapsedSeconds());

		m_input.update(last_frame_time);
		m_logic.update();
		m_graphics.run(
			last_frame_time, 
			m_camera.look_at_right_handed(), 
			m_camera.perspective_projection_right_handed(m_options.field_of_view, m_options.aspect_ratio(), m_options.far_plane, m_options.near_plane), 
			m_options.screen_width(), 
			m_options.screen_height()
		);
			
		frames_count++;

		if ((g_timer.GetTotalSeconds() - total_frame_time) >= 1.0f)
		{
			float const fps = (float)frames_count;
			float const mspf = 1000.0f / fps;

			fps_text = L"    fps: " + std::to_wstring(fps) + L"   mspf: " + std::to_wstring(mspf) + L" \n";

			frames_count = 0;
			total_frame_time += 1.0f;
		}
	}

	OutputDebugStringW(fps_text.c_str());

	m_input.finalize();
	m_logic.finalize();
	m_graphics.finalize();
}