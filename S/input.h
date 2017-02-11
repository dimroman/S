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

class application;
class options;

struct input
{
		input(application* const owner, options* const options);
	void update(float const last_frame_time);
private:
	void move_camera_along_axis_x(math::uint2 const cursor_position, float const last_frame_time, float const direction);
	void move_camera_along_axis_z(math::uint2 const cursor_position, float const last_frame_time, float const direction);

	void on_left_mouse_button_down(math::uint2 const cursor_position, float const last_frame_time);
	void left_mouse_button_is_down(math::uint2 const cursor_position, float const last_frame_time);
	void on_left_mouse_button_up(math::uint2 const cursor_position, float const last_frame_time);

	void resize(math::uint2 const cursor_position, float const last_frame_time);
	void quit(math::uint2 const cursor_position, float const last_frame_time);

private:
	application* m_owner = nullptr;
	options* m_options = nullptr;

	math::uint2 m_last_mouse_left_button_down_position{ unsigned(-1), unsigned(-1) };
	math::uint2 m_last_mouse_position{ 0, 0 };

	unsigned char m_game_key_bindings[game_key::count]{ 0 };
	unsigned char m_game_key_states[game_key::count]{ 0 };

	using owner_callback_type = std::function<void(math::uint2 const, float const)>;
	owner_callback_type m_game_key_is_down_callbacks[game_key::count];
	owner_callback_type m_game_key_down_callbacks[game_key::count];
	owner_callback_type m_game_key_up_callbacks[game_key::count];

	BYTE m_virtual_key_states[256];
};

#endif // #ifndef INPUT_H_INCLUDED