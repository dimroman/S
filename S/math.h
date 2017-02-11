#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <math.h>
#include <assert.h>
#include <algorithm>

namespace math {

struct int2
{
	int2(int const x, int const y) : x(x), y(y) {}
	inline bool operator==(int2 const other) { return x == other.x && y == other.y; }
	inline bool operator!=(int2 const other) { return x != other.x || y != other.y; }

	int x = 0;
	int y = 0;
};

struct uint2
{
	uint2(unsigned const x, unsigned const y) : x(x), y(y) {}
	inline bool operator==(uint2 const other) { return x == other.x && y == other.y; }
	inline bool operator!=(uint2 const other) { return x != other.x || y != other.y; }

	unsigned x = 0;
	unsigned y = 0;
};

template<typename T>
struct rectangle
{
	rectangle(T const first, T const second) :
		lower_bound(std::min(first.x, second.x), std::min(first.y, second.y)),
		upper_bound(std::max(first.x, second.x), std::max(first.y, second.y))
	{ }

	T lower_bound;
	T upper_bound;
};

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
	inline constexpr float4() = default;
	inline float4(float const default_value) { x = y = z = w = default_value; }
	inline constexpr float4(float const x, float const y, float const z, float const w) : x(x), y(y), z(z), w(w) {}
	inline constexpr float4(float3 const& other) : x(other.x), y(other.y), z(other.z), w(0.0f) {}
	
	inline float3 xyz() const { return float3{ x,y,z }; }

	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float e[4]{ 0.0f };
	};
};

inline float4& operator+=(float4& left, float4 const& right)
{
	left.x += right.x;
	left.y += right.y;
	left.z += right.z;
	left.w += right.w;
	return left;
}

inline float4& operator-=(float4& left, float4 const& right)
{
	left.x -= right.x;
	left.y -= right.y;
	left.z -= right.z;
	left.w -= right.w;
	return left;
}

struct float2x2
{
	inline float2x2() = default;
	inline float2x2(float const m00, float const m10, float const m01, float const m11) :
		m00(m00), m10(m10), m01(m01), m11(m11)
	{ }

	inline float2x2 operator*(float2x2 const& other) const
	{
		float2x2 result;
		result.m00 = m00 * other.m00 + m10 * other.m01;
		result.m01 = m00 * other.m10 + m10 * other.m11;
		result.m10 = m01 * other.m00 + m11 * other.m01;
		result.m11 = m01 * other.m10 + m11 * other.m11;
		return result;
	}
	inline float2x2 operator+(float2x2 const& other) const
	{
		float2x2 result;
		result.m00 = m00 + other.m00;
		result.m01 = m01 + other.m01;
		result.m10 = m10 + other.m10;
		result.m11 = m11 + other.m11;
		return result;
	}
	inline float2x2 operator-(float2x2 const& other) const
	{
		float2x2 result;
		result.m00 = m00 - other.m00;
		result.m01 = m01 - other.m01;
		result.m10 = m10 - other.m10;
		result.m11 = m11 - other.m11;
		return result;
	}
	union {
		struct {
			float m00, m10, m01, m11;
		};
		float m[2][2]{ 0 };
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
	constexpr float4x4() :
		_11(0.0f), _21(0.0f), _31(0.0f), _41(0.0f),
		_12(0.0f), _22(0.0f), _32(0.0f), _42(0.0f),
		_13(0.0f), _23(0.0f), _33(0.0f), _43(0.0f),
		_14(0.0f), _24(0.0f), _34(0.0f), _44(0.0f)
	{}

	float4x4(float4 const x, float4 const y, float4 const z, float4 const w)
	{
		columns[0] = x;
		columns[1] = y;
		columns[2] = z;
		columns[3] = w;
	}
	constexpr float4x4(
		float const c11, float const c21, float const c31, float const c41,
		float const c12, float const c22, float const c32, float const c42,
		float const c13, float const c23, float const c33, float const c43,
		float const c14, float const c24, float const c34, float const c44
	) :
		_11(c11), _21(c21), _31(c31), _41(c41),
		_12(c12), _22(c22), _32(c32), _42(c42),
		_13(c13), _23(c23), _33(c33), _43(c43),
		_14(c14), _24(c24), _34(c34), _44(c44)
	{}

	union
	{
		struct
		{
			float _11, _21, _31, _41;
			float _12, _22, _32, _42;
			float _13, _23, _33, _43;
			float _14, _24, _34, _44;
		};
		float4 columns[4];
		float m[4][4]{ 0.f };
	};

	static constexpr float4x4 identity()
	{
		return{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

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
	float const A1100 = left._11;
	float const A1101 = left._21;
	float const A1110 = left._12;
	float const A1111 = left._22;

	float const A1200 = left._13;
	float const A1201 = left._23;
	float const A1210 = left._14;
	float const A1211 = left._24;

	float const A2100 = left._31;
	float const A2101 = left._41;
	float const A2110 = left._32;
	float const A2111 = left._42;

	float const A2200 = left._33;
	float const A2201 = left._43;
	float const A2210 = left._34;
	float const A2211 = left._44;

	float const B1100 = right._11;
	float const B1101 = right._21;
	float const B1110 = right._12;
	float const B1111 = right._22;

	float const B1200 = right._13;
	float const B1201 = right._23;
	float const B1210 = right._14;
	float const B1211 = right._24;

	float const B2100 = right._31;
	float const B2101 = right._41;
	float const B2110 = right._32;
	float const B2111 = right._42;

	float const B2200 = right._33;
	float const B2201 = right._43;
	float const B2210 = right._34;
	float const B2211 = right._44;

	float const S100 = A2100 + A2200;
	float const S101 = A2101 + A2201;
	float const S110 = A2110 + A2210;
	float const S111 = A2111 + A2211;

	float const S200 = S100 - A1100;
	float const S201 = S101 - A1101;
	float const S210 = S110 - A1110;
	float const S211 = S111 - A1111;

	float const S300 = A1100 - A2100;
	float const S301 = A1101 - A2101;
	float const S310 = A1110 - A2110;
	float const S311 = A1111 - A2111;

	float const S400 = A1200 - S200;
	float const S401 = A1201 - S201;
	float const S410 = A1210 - S210;
	float const S411 = A1211 - S211;

	float const S500 = B1200 - B1100;
	float const S501 = B1201 - B1101;
	float const S510 = B1210 - B1110;
	float const S511 = B1211 - B1111;

	float const S600 = B2200 - S500;
	float const S601 = B2201 - S501;
	float const S610 = B2210 - S510;
	float const S611 = B2211 - S511;

	float const S700 = B2200 - B2100;
	float const S701 = B2201 - B2101;
	float const S710 = B2210 - B2110;
	float const S711 = B2211 - B2111;

	float const S800 = S600 - B2100;
	float const S801 = S601 - B2101;
	float const S810 = S610 - B2110;
	float const S811 = S611 - B2111;

	float const P100 = S200 * S600 + S210 * S601;
	float const P101 = S200 * S610 + S210 * S611;
	float const P110 = S201 * S600 + S211 * S601;
	float const P111 = S201 * S610 + S211 * S611;

	float const P200 = A1100 * B1100 + A1110 * B1101;
	float const P201 = A1100 * B1110 + A1110 * B1111;
	float const P210 = A1101 * B1100 + A1111 * B1101;
	float const P211 = A1101 * B1110 + A1111 * B1111;

	float const P300 = A1200 * B2100 + A1210 * B2101;
	float const P301 = A1200 * B2110 + A1210 * B2111;
	float const P310 = A1201 * B2100 + A1211 * B2101;
	float const P311 = A1201 * B2110 + A1211 * B2111;

	float const P400 = S300 * S700 + S310 * S701;//
	float const P401 = S300 * S710 + S310 * S711;//
	float const P410 = S301 * S700 + S311 * S701;//
	float const P411 = S301 * S710 + S311 * S711;//

	float const P500 = S100 * S500 + S110 * S501;
	float const P501 = S100 * S510 + S110 * S511;
	float const P510 = S101 * S500 + S111 * S501;
	float const P511 = S101 * S510 + S111 * S511;

	float const P600 = S400 * B2200 + S410 * B2201;//
	float const P601 = S400 * B2210 + S410 * B2211;//
	float const P610 = S401 * B2200 + S411 * B2201;//
	float const P611 = S401 * B2210 + S411 * B2211;//

	float const P700 = A2200 * S800 + A2210 * S801;//
	float const P701 = A2200 * S810 + A2210 * S811;//
	float const P710 = A2201 * S800 + A2211 * S801;//
	float const P711 = A2201 * S810 + A2211 * S811;//

	float const T100 = P100 + P200;
	float const T101 = P101 + P201;
	float const T110 = P110 + P210;
	float const T111 = P111 + P211;

	float const T200 = T100 + P400;
	float const T201 = T101 + P401;
	float const T210 = T110 + P410;
	float const T211 = T111 + P411;

	float4x4 result;
	result._11 = P200 + P300;
	result._12 = P210 + P310;
	result._21 = P201 + P301;
	result._22 = P211 + P311;

	result._31 = T100 + P500 + P600;
	result._32 = T110 + P510 + P610;
	result._41 = T101 + P501 + P601;
	result._42 = T111 + P511 + P611;

	result._13 = T200 - P700;
	result._14 = T210 - P710;
	result._23 = T201 - P701;
	result._24 = T211 - P711;

	result._33 = T200 + P500;
	result._34 = T210 + P510;
	result._43 = T201 + P501;
	result._44 = T211 + P511;

	//float2x2 const M1 = (A11 + A22)*(B11 + B22);
	//float2x2 const M2 = (A21 + A22)*B11;
	//float2x2 const M3 = A11*(B12 - B22);
	//float2x2 const M4 = A22*(B21 - B11);
	//float2x2 const M5 = (A11 + A12)*B22;
	//float2x2 const M6 = (A21 - A11)*(B11 + B12);
	//float2x2 const M7 = (A12 - A22)*(B21 + B22);
	//
	//float4x4 result;
	//result._11 = M1.m00 + M4.m00 - M5.m00 + M7.m00;
	//result._12 = M1.m10 + M4.m10 - M5.m10 + M7.m10;
	//result._21 = M1.m01 + M4.m01 - M5.m01 + M7.m01;
	//result._22 = M1.m11 + M4.m11 - M5.m11 + M7.m11;
	//
	//result._31 = M3.m00 + M5.m00;
	//result._32 = M3.m10 + M5.m10;
	//result._41 = M3.m01 + M5.m01;
	//result._42 = M3.m11 + M5.m11;
	//
	//result._13 = M2.m00 + M4.m00;
	//result._14 = M2.m10 + M4.m10;
	//result._23 = M2.m01 + M4.m01;
	//result._24 = M2.m11 + M4.m11;
	//
	//result._33 = M1.m00 - M2.m00 + M3.m00 + M6.m00;
	//result._34 = M1.m10 - M2.m10 + M3.m10 + M6.m10;
	//result._43 = M1.m01 - M2.m01 + M3.m01 + M6.m01;
	//result._44 = M1.m11 - M2.m11 + M3.m11 + M6.m11;

	//float4x4 result;
	//for (int i = 0; i < 4; ++i)
	//for (int j = 0; j < 4; ++j)
	//{
	//	result.m[i][j] = left.m[0][i] * right.m[j][0] + left.m[1][i] * right.m[j][1] + left.m[2][i] * right.m[j][2] + left.m[3][i] * right.m[j][3];
	//}

	return result;
}

inline void multiply(float4x4 const& left, float4x4 const& right, float4x4& result)
{
	float const& A1100 = left._11;
	float const& A1101 = left._21;
	float const& A1110 = left._12;
	float const& A1111 = left._22;

	float const& A1200 = left._13;
	float const& A1201 = left._23;
	float const& A1210 = left._14;
	float const& A1211 = left._24;

	float const& A2100 = left._31;
	float const& A2101 = left._41;
	float const& A2110 = left._32;
	float const& A2111 = left._42;

	float const& A2200 = left._33;
	float const& A2201 = left._43;
	float const& A2210 = left._34;
	float const& A2211 = left._44;

	float const& B1100 = right._11;
	float const& B1101 = right._21;
	float const& B1110 = right._12;
	float const& B1111 = right._22;

	float const& B1200 = right._13;
	float const& B1201 = right._23;
	float const& B1210 = right._14;
	float const& B1211 = right._24;

	float const& B2100 = right._31;
	float const& B2101 = right._41;
	float const& B2110 = right._32;
	float const& B2111 = right._42;

	float const& B2200 = right._33;
	float const& B2201 = right._43;
	float const& B2210 = right._34;
	float const& B2211 = right._44;

	float const S100 = A2100 + A2200;
	float const S101 = A2101 + A2201;
	float const S110 = A2110 + A2210;
	float const S111 = A2111 + A2211;

	float const S200 = S100 - A1100;
	float const S201 = S101 - A1101;
	float const S210 = S110 - A1110;
	float const S211 = S111 - A1111;

	float const S300 = A1100 - A2100;
	float const S301 = A1101 - A2101;
	float const S310 = A1110 - A2110;
	float const S311 = A1111 - A2111;

	float const S400 = A1200 - S200;
	float const S401 = A1201 - S201;
	float const S410 = A1210 - S210;
	float const S411 = A1211 - S211;

	float const S500 = B1200 - B1100;
	float const S501 = B1201 - B1101;
	float const S510 = B1210 - B1110;
	float const S511 = B1211 - B1111;

	float const S600 = B2200 - S500;
	float const S601 = B2201 - S501;
	float const S610 = B2210 - S510;
	float const S611 = right._44 - S511;

	float const S700 = B2200 - B2100;
	float const S701 = B2201 - B2101;
	float const S710 = B2210 - B2110;
	float const S711 = right._44 - B2111;

	float const S800 = S600 - B2100;
	float const S801 = S601 - B2101;
	float const S810 = S610 - B2110;
	float const S811 = S611 - B2111;
	
	float const P200 = A1100 * B1100 + A1110 * B1101;
	float const P201 = A1100 * B1110 + A1110 * B1111;
	float const P210 = A1101 * B1100 + A1111 * B1101;
	float const P211 = A1101 * B1110 + A1111 * B1111;
	
	float const P500 = S100 * S500 + S110 * S501;
	float const P501 = S100 * S510 + S110 * S511;
	float const P510 = S101 * S500 + S111 * S501;
	float const P511 = S101 * S510 + S111 * S511;

	float const T100 = S200 * S600 + S210 * S601 + P200;
	float const T101 = S200 * S610 + S210 * S611 + P201;
	float const T110 = S201 * S600 + S211 * S601 + P210;
	float const T111 = S201 * S610 + S211 * S611 + P211;

	float const T200 = T100 + S300 * S700 + S310 * S701;
	float const T201 = T101 + S300 * S710 + S310 * S711;
	float const T210 = T110 + S301 * S700 + S311 * S701;
	float const T211 = T111 + S301 * S710 + S311 * S711;

	result._11 = P200 + A1200 * B2100 + A1210 * B2101;
	result._12 = P201 + A1200 * B2110 + A1210 * B2111;
	result._21 = P210 + A1201 * B2100 + A1211 * B2101;
	result._22 = P211 + A1201 * B2110 + A1211 * B2111;

	result._13 = T100 + P500 + S400 * B2200 + S410 * B2201;
	result._14 = T101 + P501 + S400 * B2210 + S410 * B2211;
	result._23 = T110 + P510 + S401 * B2200 + S411 * B2201;
	result._24 = T111 + P511 + S401 * B2210 + S411 * B2211;

	result._31 = T200 - A2200 * S800 - A2210 * S801;
	result._32 = T201 - A2200 * S810 - A2210 * S811;
	result._41 = T210 - A2201 * S800 - A2211 * S801;
	result._42 = T211 - A2201 * S810 - A2211 * S811;

	result._33 = T200 + P500;
	result._43 = T210 + P510;
	result._34 = T201 + P501;
	result._44 = T211 + P511;

	//float2x2 const M1 = (A11 + A22)*(B11 + B22);
	//float2x2 const M2 = (A21 + A22)*B11;
	//float2x2 const M3 = A11*(B12 - B22);
	//float2x2 const M4 = A22*(B21 - B11);
	//float2x2 const M5 = (A11 + A12)*B22;
	//float2x2 const M6 = (A21 - A11)*(B11 + B12);
	//float2x2 const M7 = (A12 - A22)*(B21 + B22);
	//
	//float4x4 result;
	//result._11 = M1.m00 + M4.m00 - M5.m00 + M7.m00;
	//result._12 = M1.m10 + M4.m10 - M5.m10 + M7.m10;
	//result._21 = M1.m01 + M4.m01 - M5.m01 + M7.m01;
	//result._22 = M1.m11 + M4.m11 - M5.m11 + M7.m11;
	//
	//result._31 = M3.m00 + M5.m00;
	//result._32 = M3.m10 + M5.m10;
	//result._41 = M3.m01 + M5.m01;
	//result._42 = M3.m11 + M5.m11;
	//
	//result._13 = M2.m00 + M4.m00;
	//result._14 = M2.m10 + M4.m10;
	//result._23 = M2.m01 + M4.m01;
	//result._24 = M2.m11 + M4.m11;
	//
	//result._33 = M1.m00 - M2.m00 + M3.m00 + M6.m00;
	//result._34 = M1.m10 - M2.m10 + M3.m10 + M6.m10;
	//result._43 = M1.m01 - M2.m01 + M3.m01 + M6.m01;
	//result._44 = M1.m11 - M2.m11 + M3.m11 + M6.m11;

	//float4x4 result;
	//for (int i = 0; i < 4; ++i)
	//for (int j = 0; j < 4; ++j)
	//{
	//	result.m[i][j] = left.m[0][i] * right.m[j][0] + left.m[1][i] * right.m[j][1] + left.m[2][i] * right.m[j][2] + left.m[3][i] * right.m[j][3];
	//}
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