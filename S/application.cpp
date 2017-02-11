#include "application.h"
#include <string>
#include "StepTimer.h"

StepTimer g_timer;

application::application() :
	m_options(&m_graphics),
	m_graphics(m_options.screen_width(), m_options.screen_height()),
	m_camera({ 0.0f, 0.0f, 0.0f }),
	m_logic(m_graphics),
	m_input(&m_camera, &m_options, &m_graphics)
{

}

void application::run()
{		
	MSG msg;
	unsigned frames_count = 0;
	float total_frame_time = 0.0f;
	std::wstring fps_text;
	
	bool quit = false;
	while (!quit)
	{
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{}
		g_timer.Tick(NULL);
		float const last_frame_time = static_cast<float>(g_timer.GetElapsedSeconds());

		m_input.update(last_frame_time, quit);
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
}