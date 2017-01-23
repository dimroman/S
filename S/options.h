#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

struct options
{
	int const screen_width = 1920;
	int const screen_height = 1080;

	float const field_of_view = 60.0f;

	float const far_plane = 125.0f;
	float const near_plane = 0.01f;

	inline float aspect_ratio() const { return static_cast<float>(screen_width) / static_cast<float>(screen_height); }
};

#endif // #ifndef OPTIONS_H_INCLUDED