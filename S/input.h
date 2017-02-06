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
	count
};

}

class application;

struct input
{
	void initialize(application* const owner);
	void update(float const last_frame_time);
private:
	void move_camera_along_axis_x(POINT const cursor_position, float const last_frame_time, float const direction);
	void move_camera_along_axis_z(POINT const cursor_position, float const last_frame_time, float const direction);

	void on_left_mouse_button_down(POINT const cursor_position, float const last_frame_time);
	void on_left_mouse_button_up(POINT const cursor_position, float const last_frame_time);

private:
	application* m_owner = nullptr;
	bool m_mouse_is_down = false;

	float m_last_dx = 0;
	float m_last_dy = 0;
	POINT m_last_mouse_left_button_down_position{ 0, 0 };
	POINT m_last_mouse_position{ 0, 0 };

	unsigned char m_game_key_bindings[game_key::count];
	unsigned char m_game_key_states[game_key::count];

	using owner_callback_type = std::function<void(POINT const, float const)>;
	owner_callback_type m_game_key_is_down_callbacks[game_key::count];
	owner_callback_type m_game_key_down_callbacks[game_key::count];
	owner_callback_type m_game_key_up_callbacks[game_key::count];

	BYTE m_virtual_key_states[256];
};

#endif // #ifndef INPUT_H_INCLUDED