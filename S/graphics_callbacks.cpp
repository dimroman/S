#include "graphics_callbacks.h"
#include "math.h"

void empty_selection_updated_callback(bool const is_selected, bool const is_highlighted, math::float4& color)
{

}

void default_selection_updated_callback(bool const is_selected, bool const is_highlighted, math::float4& color)
{
	if (is_highlighted)
		color = { 0.25f, 0.25f, 0.25f, 1.0f };
	else if (is_selected)
		color = { 0.0f, 0.0f, 0.0f, 1.0f };
	else
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
}