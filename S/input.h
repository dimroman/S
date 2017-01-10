#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "math.h"
#include <windows.h>

struct input
{
	void on_key_down(UINT8 const key);
	void on_key_up(UINT8 const key);

	void on_mouse_down(WPARAM const button_state, int const x, int const y);
	void on_mouse_up(WPARAM const button_state, int const x, int const y);
	void on_mouse_move(WPARAM const button_state, int const x, int const y);

	void reset();

	int last_mouse_x = 0;
	int last_mouse_y = 0;
	float last_dx = 0;
	float last_dy = 0;

	bool rightArrowPressed = false;
	bool leftArrowPressed = false;
	bool forward_arrow_pressed = false;
	bool backward_arrow_pressed = false;
	bool animate = true;
};

#endif // #ifndef INPUT_H_INCLUDED