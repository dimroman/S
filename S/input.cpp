#include "input.h"
#include <Windows.h>
#include "camera.h"
#include "options.h"
#include "graphics.h"

enum {
	virtual_key_left_mouse_button = 0x01,
	virtual_key_right_mouse_button = 0x02,
	virtual_key_cancel = 0x03,
	virtual_key_middle_mouse_button = 0x04,
	virtual_key_x1_mouse_button = 0x05,
	virtual_key_x2_mouse_button = 0x06,

	virtual_key_backspace = 0x08,
	virtual_key_tab = 0x09,

	virtual_key_clear = 0x0C,
	virtual_key_return = 0x0D,

	virtual_key_shift = 0x10,
	virtual_key_control = 0x11,
	virtual_key_alt = 0x12,
	virtual_key_pause = 0x13,
	virtual_key_caps_lock = 0x14,
	virtual_key_kana = 0x15,
	virtual_key_hanguel = 0x15,
	virtual_key_hangul = 0x15,
	
	virtual_key_junja = 0x17,
	virtual_key_final = 0x18,
	virtual_key_hanja = 0x19,
	virtual_key_kanji = 0x19,

	virtual_key_escape = 0x1B,
	virtual_key_convert = 0x1C,
	virtual_key_nonconvert = 0x1D,
	virtual_key_accept = 0x1E,
	virtual_key_modechange = 0x1F,
	virtual_key_space = 0x20,
	virtual_key_page_up = 0x21,
	virtual_key_next = 0x22,
	virtual_key_end = 0x23,
	virtual_key_home = 0x24,
	virtual_key_left_arrow = 0x25,
	virtual_key_up_arrow = 0x26,
	virtual_key_right_arrow = 0x27,
	virtual_key_down_arrow = 0x28,
	virtual_key_select = 0x29,
	virtual_key_print = 0x2A,
	virtual_key_execute = 0x2B,
	virtual_key_print_screen = 0x2C,
	virtual_key_insert = 0x2D,
	virtual_key_delete = 0x2E,
	virtual_key_help = 0x2F,
	virtual_key_0 = 0x30,
	virtual_key_1 = 0x31,
	virtual_key_2 = 0x32,
	virtual_key_3 = 0x33,
	virtual_key_4 = 0x34,
	virtual_key_5 = 0x35,
	virtual_key_6 = 0x36,
	virtual_key_7 = 0x37,
	virtual_key_8 = 0x38,
	virtual_key_9 = 0x39,
	
	virtual_key_a = 0x41,
	virtual_key_b = 0x42,
	virtual_key_c = 0x43,
	virtual_key_d = 0x44,
	virtual_key_e = 0x45,
	virtual_key_f = 0x46,
	virtual_key_g = 0x47,
	virtual_key_h = 0x48,
	virtual_key_i = 0x49,
	virtual_key_j = 0x4A,
	virtual_key_k = 0x4B,
	virtual_key_l = 0x4C,
	virtual_key_m = 0x4D,
	virtual_key_n = 0x4E,
	virtual_key_o = 0x4F,
	virtual_key_p = 0x50,
	virtual_key_q = 0x51,
	virtual_key_r = 0x52,
	virtual_key_s = 0x53,
	virtual_key_t = 0x54,
	virtual_key_u = 0x55,
	virtual_key_v = 0x56,
	virtual_key_w = 0x57,
	virtual_key_x = 0x58,
	virtual_key_y = 0x59,
	virtual_key_z = 0x5A,
	virtual_key_left_windows = 0x5B,
	virtual_key_right_windows = 0x5C,
	virtual_key_applications = 0x5D,

	virtual_key_sleep = 0x5F,
	virtual_key_numpad_0 = 0x60,
	virtual_key_numpad_1 = 0x61,
	virtual_key_numpad_2 = 0x62,
	virtual_key_numpad_3 = 0x63,
	virtual_key_numpad_4 = 0x64,
	virtual_key_numpad_5 = 0x65,
	virtual_key_numpad_6 = 0x66,
	virtual_key_numpad_7 = 0x67,
	virtual_key_numpad_8 = 0x68,
	virtual_key_numpad_9 = 0x69,
	virtual_key_multiply = 0x6A,
	virtual_key_add = 0x6B,
	virtual_key_separator = 0x6C,
	virtual_key_subtract = 0x6D,
	virtual_key_decimal = 0x6E,
	virtual_key_divide = 0x6F,
	virtual_key_f1 = 0x70,
	virtual_key_f2 = 0x71,
	virtual_key_f3 = 0x72,
	virtual_key_f4 = 0x73,
	virtual_key_f5 = 0x74,
	virtual_key_f6 = 0x75,
	virtual_key_f7 = 0x76,
	virtual_key_f8 = 0x77,
	virtual_key_f9 = 0x78,
	virtual_key_f10 = 0x79,
	virtual_key_f11 = 0x7A,
	virtual_key_f12 = 0x7B,
	virtual_key_f13 = 0x7C,
	virtual_key_f14 = 0x7D,
	virtual_key_f15 = 0x7E,
	virtual_key_f16 = 0x7F,
	virtual_key_f17 = 0x80,
	virtual_key_f18 = 0x81,
	virtual_key_f19 = 0x82,
	virtual_key_f20 = 0x83,
	virtual_key_f21 = 0x84,
	virtual_key_f22 = 0x85,
	virtual_key_f23 = 0x86,
	virtual_key_f24 = 0x87,

	virtual_key_numlock = 0x90,
	virtual_key_scroll_lock = 0x91,

	virtual_key_left_shift = 0xA0,
	virtual_key_right_shift = 0xA1,
	virtual_key_left_control = 0xA2,
	virtual_key_right_control = 0xA3,
	virtual_key_left_menu = 0xA4,
	virtual_key_right_menu = 0xA5,
	virtual_key_browser_back = 0xA6,
	virtual_key_browser_forward = 0xA7,
	virtual_key_browser_refresh = 0xA8,
	virtual_key_browser_stop = 0xA9,
	virtual_key_browser_search = 0xAA,
	virtual_key_browser_favorites = 0xAB,
	virtual_key_browser_home = 0xAC,
	virtual_key_volume_mute = 0xAD,
	virtual_key_volume_down = 0xAE,
	virtual_key_volume_up = 0xAF,
	virtual_key_media_next_track = 0xB0,
	virtual_key_media_previous_track = 0xB1,
	virtual_key_media_stop = 0xB2,
	virtual_key_media_play_pause = 0xB3,
	virtual_key_launch_mail = 0xB4,
	virtual_key_launch_media_select = 0xB5,
	virtual_key_start_application_1 = 0xB6,
	virtual_key_start_application_2 = 0xB7,

	virtual_key_oem_1 = 0xBA,
	virtual_key_oem_plus = 0xBB,
	virtual_key_oem_comma = 0xBC,
	virtual_key_oem_minus = 0xBD,
	virtual_key_oem_period = 0xBE,
	virtual_key_oem_2 = 0xBF,
	virtual_key_oem_3 = 0xC0,

	virtual_key_oem_4 = 0xDB,
	virtual_key_oem_5 = 0xDC,
	virtual_key_oem_6 = 0xDD,
	virtual_key_oem_7 = 0xDE,
	virtual_key_oem_8 = 0xDF,

	virtual_key_oem_102 = 0xE2,

	virtual_key_process_key = 0xE5,

	virtual_key_packet = 0xE7,

	virtual_key_attn = 0xF6,
	virtual_key_crsel = 0xF7,
	virtual_key_exsel = 0xF8,
	virtual_key_ereof = 0xF9,
	virtual_key_play = 0xFA,
	virtual_key_zoom = 0xFB,
	virtual_key_noname = 0xFC,
	virtual_key_pa_1 = 0xFD,
	virtual_key_oem_clear = 0xFE,
};

void input::move_camera_along_axis_x(float const direction)
{
	//m_camera->move_x(m_last_frame_time*direction);
}

void input::move_camera_along_axis_z(float const direction)
{
	m_camera->move_z(m_last_frame_time*direction);
}

void input::on_left_mouse_button_down()
{
	m_last_mouse_left_button_down_position = m_current_cursor_position;
}

void input::left_mouse_button_is_down()
{
	if (m_current_cursor_position.x != m_previous_cursor_position.x)
	{
		float const dx = math::to_radians(0.01f*static_cast<float>(m_current_cursor_position.x - m_previous_cursor_position.x));
		//m_camera->rotate_around_up(-dx);
	}
	if (m_current_cursor_position.y != m_previous_cursor_position.y)
	{
		float const dy = math::to_radians(0.01f*static_cast<float>(m_current_cursor_position.y - m_previous_cursor_position.y));
		//m_camera->rotate_around_x(dy);
	}

	if (m_last_mouse_left_button_down_position.x != unsigned(-1) && m_last_mouse_left_button_down_position.y != unsigned(-1))
	{
		math::rectangle<math::uint2> const selection(
			m_area_selection_enabled ? m_current_cursor_position : m_last_mouse_left_button_down_position, 
			m_last_mouse_left_button_down_position
		);
		m_graphics->highlight_object(selection, m_options->screen_width());
	}
}

void input::on_left_mouse_button_up()
{
	math::rectangle<math::uint2> const selection(
		m_area_selection_enabled ? m_current_cursor_position : m_last_mouse_left_button_down_position, 
		m_last_mouse_left_button_down_position
	);
	m_graphics->select_object(selection, m_options->screen_width());
	m_graphics->remove_all_highlighting();
}

input::input(camera* const camera, options* const options, graphics* const graphics, bool const area_selection_enabled) :
	m_camera(camera),
	m_options(options),
	m_graphics(graphics),
	m_area_selection_enabled(area_selection_enabled)
{
	m_game_key_bindings[game_key::left_mouse_button] = virtual_key_left_mouse_button;
	m_game_key_bindings[game_key::w_keyboard_button] = virtual_key_w;
	m_game_key_bindings[game_key::a_keyboard_button] = virtual_key_a;
	m_game_key_bindings[game_key::s_keyboard_button] = virtual_key_s;
	m_game_key_bindings[game_key::d_keyboard_button] = virtual_key_d;
	m_game_key_bindings[game_key::resize_keyboard_button] = virtual_key_r;
	m_game_key_bindings[game_key::quit_keyboard_button] = virtual_key_escape;
	
	m_game_key_is_down_callbacks[game_key::left_mouse_button] = std::bind(&input::left_mouse_button_is_down, this);
	m_game_key_is_down_callbacks[game_key::w_keyboard_button] = std::bind(&input::move_camera_along_axis_z, this, -1.0f);
	m_game_key_is_down_callbacks[game_key::a_keyboard_button] = std::bind(&input::move_camera_along_axis_x, this, -1.0f);
	m_game_key_is_down_callbacks[game_key::s_keyboard_button] = std::bind(&input::move_camera_along_axis_z, this, 1.0f);
	m_game_key_is_down_callbacks[game_key::d_keyboard_button] = std::bind(&input::move_camera_along_axis_x, this, 1.0f);

	m_game_key_down_callbacks[game_key::left_mouse_button] = std::bind(&input::on_left_mouse_button_down, this);
	m_game_key_down_callbacks[game_key::resize_keyboard_button] = std::bind(&input::resize, this);
	m_game_key_down_callbacks[game_key::quit_keyboard_button] = std::bind(&input::quit, this);

	m_game_key_up_callbacks[game_key::left_mouse_button] = std::bind(&input::on_left_mouse_button_up, this);
}

void input::update(float const last_frame_time, bool& need_to_quit)
{
	m_last_frame_time = last_frame_time;
	m_previous_cursor_position = m_current_cursor_position;
	POINT point;
	BOOL const result = GetCursorPos(&point);
	assert(result);
	assert(point.x >= 0);
	assert(point.y >= 0);

	m_current_cursor_position = { 
		m_options->clamp_screen_position_x(point.x), 
		m_options->clamp_screen_position_y(point.y) 
	};

	for (unsigned i = 0; i < game_key::count; ++i)
	{
		short const state = GetAsyncKeyState(m_game_key_bindings[i]);
		if ((state >> 8) & 0xff)
		{
			if (m_game_key_is_down_callbacks[i])
				m_game_key_is_down_callbacks[i]();
			if (m_game_key_states[i] == 0)
			{
				if (m_game_key_down_callbacks[i])
					m_game_key_down_callbacks[i]();
				m_game_key_states[i] = 1;
			}
		}
		else if ( m_game_key_states[i] == 1 )
		{
			if (m_game_key_up_callbacks[i])
				m_game_key_up_callbacks[i]();
			m_game_key_states[i] = 0;
		}
	}

	need_to_quit = m_need_to_quit;
}

void input::resize()
{
	m_options->resize();
}

void input::quit()
{
	m_need_to_quit = true;
}