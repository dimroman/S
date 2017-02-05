#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "math.h"
#include <windows.h>

class application;

struct input
{
	void initialize(application* owner) { m_owner = owner; }

	void on_key_down(UINT8 const key);
	void on_key_up(UINT8 const key);

	void on_mouse_down(WPARAM const button_state, int const x, int const y);
	void on_mouse_up(WPARAM const button_state, int const x, int const y);
	void on_mouse_move(WPARAM const button_state, int const x, int const y);

	void reset();

	float last_dx = 0;
	float last_dy = 0;

	bool rightArrowPressed = false;
	bool leftArrowPressed = false;
	bool forward_arrow_pressed = false;
	bool backward_arrow_pressed = false;

private:
	application* m_owner = nullptr;
	bool m_mouse_is_down = false;
	math::int2 m_last_mouse_down_position{ 0, 0 };
	math::int2 m_last_mouse_position{ 0, 0 };
};

#endif // #ifndef INPUT_H_INCLUDED