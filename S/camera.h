#ifndef WORLD_CAMERA_H_INCLUDED
#define WORLD_CAMERA_H_INCLUDED

#include "math.h"

class world_camera {
public:
	math::float4x4 look_at_right_handed();
	math::float4x4 perspective_projection_right_handed() const;

	void set_position(math::float3 const position);
	void set_look_position(math::float3 const position);

	void move_x(float const value);
	void move_y(float const value);
	void move_z(float const value);

	void rotate_around_up(float const radians);
	void rotate_around_x(float const radians);

private:
	math::float3 m_position{ 0.0f, 15.0f, -30.0f };

	math::float3 m_axis_x{ 1.0f, 0.0f, 0.0f };
	math::float3 m_axis_y{ 0.0f, 1.0f, 0.0f };
	math::float3 m_axis_z{ 0.0f, 0.0f, -1.0f };

	bool m_normalize_axis = true;
};

#endif // #ifndef WORLD_CAMERA_H_INCLUDED