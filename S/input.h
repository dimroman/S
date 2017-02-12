#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "math.h"
#include <windows.h>
#include <functional>

namespace game_key {
	
enum {
	left_mouse_button,
	w_keyboard_button,
	a_keyboard_button,
	s_keyboard_button,
	d_keyboard_button,
	resize_keyboard_button,
	quit_keyboard_button,
	count
};

}

class camera;
class options;
class graphics;

struct input
{
		input(camera* const camera, options* const options, graphics* const graphics, bool const area_selection_enabled);
	void update(float const last_frame_time, bool& need_to_quit);
private:
	void move_camera_along_axis_x(float const direction);
	void move_camera_along_axis_z(float const direction);

	void on_left_mouse_button_down();
	void left_mouse_button_is_down();
	void on_left_mouse_button_up();

	void resize();
	void quit();

private:
	camera* const m_camera = nullptr;
	graphics* const m_graphics = nullptr;
	options* const m_options = nullptr;

	float m_last_frame_time = 0.0f;
	math::uint2 m_previous_cursor_position{ 0, 0 };
	math::uint2 m_current_cursor_position{ 0, 0 };

	math::uint2 m_last_mouse_left_button_down_position{ unsigned(-1), unsigned(-1) };

	unsigned char m_game_key_bindings[game_key::count]{ 0 };
	unsigned char m_game_key_states[game_key::count]{ 0 };

	using owner_callback_type = std::function<void()>;
	owner_callback_type m_game_key_is_down_callbacks[game_key::count];
	owner_callback_type m_game_key_down_callbacks[game_key::count];
	owner_callback_type m_game_key_up_callbacks[game_key::count];

	BYTE m_virtual_key_states[256];

	bool const m_area_selection_enabled = false;
	bool m_need_to_quit = false;
};

#endif // #ifndef INPUT_H_INCLUDED