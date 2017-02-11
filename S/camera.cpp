#include "camera.h"

camera::camera(math::float3 const position)
{
	set_look_position(position);
}

math::float4x4 camera::look_at_right_handed()
{
	if (m_normalize_axis)
	{
		m_axis_x = math::normalize(m_axis_x);
		m_axis_z = math::normalize(math::cross(m_axis_x, m_axis_y));
		m_axis_y = math::cross(m_axis_z, m_axis_x);

		m_normalize_axis = false;
	}

	math::float4x4 orientation{
		math::float4{ m_axis_x.x, m_axis_x.y, m_axis_x.z, 0.0f },
		math::float4{ m_axis_y.x, m_axis_y.y, m_axis_y.z, 0.0f },
		math::float4{ m_axis_z.x, m_axis_z.y, m_axis_z.z, 0.0f },
		math::float4{ 0.0f, 0.0f, 0.0f, 1.0f }
	};

	math::float4x4 translation{
		math::float4{ 1.0f, 0.0f, 0.0f, 0.0f },
		math::float4{ 0.0f, 1.0f, 0.0f, 0.0f },
		math::float4{ 0.0f, 0.0f, 1.0f, 0.0f },
		math::float4{ -m_position.x, -m_position.y, -m_position.z, 1.0f }
	};

	math::float4x4 result;
	math::multiply(orientation, translation, result);

	return result;
}

math::float4x4 camera::perspective_projection_right_handed(float const field_of_view, float const aspect_ratio, float const far_plane, float const near_plane) const
{
	math::float4x4 result;

	result.m[1][1] = 1 / (float)tan(0.5f * math::to_radians( field_of_view ));
	result.m[0][0] = result.m[1][1] / aspect_ratio;
	result.m[2][2] = -far_plane / (far_plane - near_plane);
	result.m[3][2] = -far_plane * near_plane / (far_plane - near_plane);
	result.m[2][3] = -1.0f;

	return result;
}

void camera::set_position(math::float3 const position)
{ 
	m_position = position; 
}

void camera::set_look_position(math::float3 const position)
{
	m_axis_z = math::normalize(m_position - position);
	m_axis_x = math::normalize(cross(m_axis_y, m_axis_z));
	m_axis_y = math::normalize(cross(m_axis_z, m_axis_x));	
}

void camera::move_x(float const value)
{
	m_position = m_position + m_axis_x*value*10;
}

void camera::move_y(float const value)
{
	m_position = m_position + m_axis_y*value*10;
}

void camera::move_z(float const value)
{
	m_position = m_position + m_axis_z*value * 10;
}

void camera::rotate_around_up(float const radians)
{
	math::float3x3 rotation = math::float3x3::rotation_around_axis_y(radians);

	m_axis_x = math::normalize(m_axis_x*rotation);
	m_axis_y = math::normalize(m_axis_y*rotation);
	m_axis_z = math::normalize(m_axis_z*rotation);

	m_normalize_axis = true;
}

void camera::rotate_around_x(float const radians)
{
	math::float3x3 rotation = math::float3x3::rotation( m_axis_x, radians);

	m_axis_y = math::normalize(m_axis_y*rotation);
	m_axis_z = math::normalize(m_axis_z*rotation);

	m_normalize_axis = true;
}