#ifndef GLOBAL_DEFINES_H_INCLUDED
#define GLOBAL_DEFINES_H_INCLUDED

enum
{
	frames_count = 2,
};

enum selection_mask_values
{
	object_in_default_state = 1 << 0,
	object_is_selected = 1 << 1,
	object_is_highlighted = 1 << 2,
	neighbour_is_selected = 1 << 3,
	neighbour_is_highlighted = 1 << 4,
};

#endif // #ifndef GLOBAL_DEFINES_H_INCLUDED