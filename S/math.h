#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <math.h>
#include <assert.h>

namespace math {

inline float to_radians(float const degrees) { return degrees * 3.1415927f / 180.0f; };
inline float to_degrees(float const radians) { return radians * 180.0f / 3.1415927f; }

struct float2
{
	float x;
	float y;
};

struct float3
{
	float3() = default;
	float3(float const default_value) { x = y = z = default_value; }
	float3(float const x, float const y, float const z) : x(x), y(y), z(z) {}

	inline float3 operator/(float const divider) const
	{
		assert(divider != 0.0f);
		return{ x / divider, y / divider, z / divider };
	}

	union {
		struct {
			float x;
			float y;
			float z;
		};
		float e[3];
	};
};


inline float squared_length(float3 const& vector)
{
	return vector.x*vector.x + vector.y*vector.y + vector.z*vector.z;
}

inline float length(float3 const& vector)
{
	return (float)sqrt(squared_length(vector));
}

inline float3 operator+(float3 const& left, float3 const& right)
{
	return float3{ left.x + right.x, left.y + right.y, left.z + right.z };
}

inline float3 operator-(float3 const& left, float3 const& right)
{
	return float3{ left.x - right.x, left.y - right.y, left.z - right.z };
}

inline float3 operator*(float3 const& vector, float const value)
{
	return{ vector.x*value, vector.y*value, vector.z*value };
}

inline float3 operator*(float const value, float3 const& vector)
{
	return vector*value;
}

inline float dot_product(float3 const& left, float3 const& right)
{
	return left.x*right.x + left.y*right.y + left.z*right.z;
}

inline float3 normalize(float3 const& vector)
{
	return vector / length( vector );
}

inline float3 cross(float3 const& left, float3 const right)
{
	float3 result;
	result.x = left.y*right.z - left.z*right.y;
	result.y = left.z*right.x - left.x*right.z;
	result.z = left.x*right.y - left.y*right.x;
	return result;
}

struct float4
{
	float4() = default;
	float4(float const default_value) { x = y = z = w = default_value; }
	float4(float const x, float const y, float const z, float const w) : x(x), y(y), z(z), w(w) {}
	float4(float3 const& other) : x(other.x), y(other.y), z(other.z), w(0.0f) {}
	inline float3 xyz() const { return float3{ x,y,z }; }

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float e[4];
	};
};

struct float3x3
{
	float3x3() = default;
	float3x3(float3 const x, float3 const y, float3 const z)
	{
		columns[0] = x;
		columns[1] = y;
		columns[2] = z;
	}

	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float3 columns[3];
		float m[3][3]{ 0.f };
	};

	static inline float3x3 rotation_around_axis_x(float const radians)
	{
		float3x3 result;
		result.m[0][0] = 1.0f;
		result.m[1][1] = (float)cos(radians);
		result.m[1][2] = (float)sin(radians);
		result.m[2][1] = -(float)sin(radians);
		result.m[2][2] = (float)cos(radians);
		return result;
	}

	static inline float3x3 rotation_around_axis_y(float const radians)
	{
		float3x3 result;
		result.m[0][0] = (float)cos(radians);
		result.m[0][2] = -(float)sin(radians);
		result.m[2][0] = (float)sin(radians);
		result.m[1][1] = 1.0f;
		result.m[2][2] = (float)cos(radians);
		return result;
	}

	static inline float3x3 rotation_around_axis_z(float const radians)
	{
		float3x3 result;
		result.m[0][0] = (float)cos(radians);
		result.m[0][1] = (float)sin(radians);
		result.m[1][0] = -(float)sin(radians);
		result.m[1][1] = (float)cos(radians);
		result.m[2][2] = 1.0f;
		return result;
	}

	static inline float3x3 rotation(float3 const axis, float const radians)
	{
		float const c = (float)cos(radians);
		float const s = (float)sin(radians);
		float const x = axis.x;
		float const y = axis.y;
		float const z = axis.z;

		return float3x3{
			{ c + (1-c)*x*x, (1-c)*x*y - s*z, (1-c)*x*z + s*y },
			{ (1-c)*x*y + s*z, c + (1-c)*y*y, (1-c)*y*z - s*x },
			{ (1-c)*x*z - s*y, (1-c)*y*z + s*z, c + (1-c)*z*z }
		};
	}
};

inline float3 operator*(float3 const vector, float3x3 const& matrix)
{
	float3 result(0.0f);
	for (int i = 0; i < 3; ++i)
	for (int j = 0; j < 3; ++j)
	{
		result.e[i] += vector.e[j] * matrix.m[j][i];
	}
	return result;
}

inline float3 operator*(float3x3 const& matrix, float3 const vector)
{
	float3 result(0.0f);
	for (int i = 0; i < 3; ++i)
	for (int j = 0; j < 3; ++j)
	{
		result.e[i] += matrix.m[i][j] * vector.e[j];
	}
	return result;
}

inline float determinant(float3x3 const& m)
{
	return m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1])
		 - m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0])
		 + m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]);
}

struct float4x4
{
	float4x4() = default;
	float4x4(float4 const x, float4 const y, float4 const z, float4 const w)
	{
		columns[0] = x;
		columns[1] = y;
		columns[2] = z;
		columns[3] = w;
	}

	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float4 columns[4];
		float m[4][4]{ 0.f };
	};

	static inline float4x4 rotation_around_axis_x(float const radians)
	{
		float4x4 result;
		result.m[0][0] = 1.0f;
		result.m[1][1] = (float)cos(radians);
		result.m[1][2] = (float)sin(radians);
		result.m[2][1] = -(float)sin(radians);
		result.m[2][2] = (float)cos(radians);
		result.m[3][3] = 1.0f;
		return result;
	}

	static inline float4x4 rotation_around_axis_y(float const radians)
	{
		float4x4 result;
		result.m[0][0] = (float)cos(radians);
		result.m[0][2] = -(float)sin(radians);
		result.m[2][0] = (float)sin(radians);
		result.m[1][1] = 1.0f;
		result.m[2][2] = (float)cos(radians);
		result.m[3][3] = 1.0f;
		return result;
	}

	static inline float4x4 rotation_around_axis_z(float const radians)
	{
		float4x4 result;
		result.m[0][0] = (float)cos(radians);
		result.m[0][1] = (float)sin(radians);
		result.m[1][0] = -(float)sin(radians);
		result.m[1][1] = (float)cos(radians);
		result.m[2][2] = 1.0f;
		result.m[3][3] = 1.0f;
		return result;
	}
};

inline float4x4 operator*(float4x4 const& left, float4x4 const& right)
{
	float4x4 result;
	for (int i = 0; i < 4; ++i)
	for (int j = 0; j < 4; ++j)
	{
		result.m[i][j] = 0.0f;
		for (int k = 0; k < 4; ++k)
		{
			result.m[i][j] += left.m[k][i] * right.m[j][k];
		}
	}
	return result;
}

inline float4x4 scale(float const x, float const y, float const z)
{
	float4x4 result;
	result.m[0][0] = x;
	result.m[1][1] = y;
	result.m[2][2] = z;
	result.m[3][3] = 1.0f;
	return result;
}

inline float4x4 identity()
{
	float4x4 result;
	result._11 = 1.0f;
	result._22 = 1.0f;
	result._33 = 1.0f;
	result._44 = 1.0f;
	return result;
}

inline float4 operator*(float4 const vector, float4x4 const& matrix)
{
	float4 result(0.0f);
	for (int i = 0; i < 4; ++i)
	for (int j = 0; j < 4; ++j)
	{
		result.e[i] += vector.e[j] * matrix.m[j][i];
	}
	return result;
}

inline float4 operator*(float4x4 const& matrix, float4 const vector)
{
	float4 result(0.0f);	
	for (int i = 0; i < 4; ++i)
	for (int j = 0; j < 4; ++j)
	{
		result.e[i] += matrix.m[i][j] * vector.e[j];
	}
	return result;
}

inline float determinant(float4x4 const& m)
{
	return
		m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
		m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0] +
		m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
		m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
		m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
		m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
		m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
		m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] +
		m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
		m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] + m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
		m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
		m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3];
}

inline float3x3 cofactor(float4x4 const& matrix, int const row, int const column)
{
	float3x3 result;
	for (int i = 0; i < 4; ++i)
	{
		if (i == row)
			continue;

		int i_c = (i < row) ? i : i - 1;

		for (int j = 0; j < 4; ++j)
		{
			if (j == column)
				continue;

			int j_c = (j < column) ? j : j - 1;
			result.m[i_c][j_c] = matrix.m[i][j];
		}
	}

	return result;
}

inline float4x4 inverse(float4x4 const& matrix)
{
	float det = determinant(matrix);
	assert(det != 0);
	float one_over_det = 1.0f / det;

	float4x4 result;
	for ( int i = 0; i < 4; ++i)
	for (int j = 0; j < 4; ++j)
	{
		result.m[i][j] = (-1 + (((i + j + 1) & 1) << 1))*determinant(cofactor(matrix, j, i))*one_over_det;
	}

	return result;
}

inline float4x4 transpose(float4x4 const& matrix)
{
	float4x4 result;
	for (int i = 0; i < 4; ++i)
	for (int j = 0; j < 4; ++j)
	{
		result.m[i][j] = matrix.m[j][i];
	}
	return result;
}

} // namespace math

#endif // #ifndef MATH_H_INCLUDED