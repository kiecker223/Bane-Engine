#pragma once
#ifndef __BANE_MATH_H__
#define __BANE_MATH_H__

#include <DirectXMath.h>
#include "Core/Common.h"

using namespace DirectX;

#define _PI_ 3.141592654f


inline uint NextPowerOfTwo(uint Value)
{
	Value--;
	Value |= Value >> 1;
	Value |= Value >> 2;
	Value |= Value >> 4;
	Value |= Value >> 8;
	Value |= Value >> 16;
	Value++;
	return Value;
}


#if 0 // All broken, very broken
/*
----- Float4 -----
*/
struct float4
{
	union
	{
#if defined (USE_FAST_MATH)
		__m128 v;
#else
		float d[4];
#endif
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
	};

	float4()
#if !defined(USE_FAST_MATH)
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
#else
	{
		v = _mm_set1_ps(0.0f);
	}
#endif
	float4(float X, float Y, float Z, float W)
#if !defined(USE_FAST_MATH)
		: x(X), y(Y), z(Z), w(W) { }
#else
	{
		v = _mm_set_ps(X, Y, Z, W);
	}
#endif

	float4(const float4& other)
#if !defined(USE_FAST_MATH)
		: x(other.x), y(other.y), z(other.z), w(other.w) { }
#else
	{
		_mm_store_ps(&v.m128_f32[0], other.v);
	}
#endif

#pragma region OPERATORS

#if defined(USE_FAST_MATH)
	inline operator __m128()
	{
		return this->v;
	}
#endif

#if defined(USE_FAST_MATH)
	inline operator __m128() const
	{
		return v;
	}
#endif

	inline float4& operator = (const float4& right)
	{
#if defined(USE_FAST_MATH)
		_mm_store_ps(&v.m128_f32[0], right.v);
#else
		for (uint i = 0; i < 4; i++)
			d[i] = right[i];
#endif
		return *this;
	}

	inline float& operator[] (uint pos)
	{
#if defined(USE_FAST_MATH)
		return v.m128_f32[pos];
#else
		return d[pos];
#endif
	}

	inline float operator[] (uint pos) const
	{
#if defined(USE_FAST_MATH)
		return (const float)v.m128_f32[pos];
#else
		return (const float)d[pos];
#endif
	}

	inline float4 operator * (const float4& right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_mul_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] * right[i];
		}
#endif
		return r;
	}

	inline float4 operator / (const float4& right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_div_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right[i];
		}
#endif
		return r;
	}

	inline float4 operator + (const float4& right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_add_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right[i];
		}
#endif
		return r;
	}

	inline float4 operator - (const float4& right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_sub_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right[i];
		}
#endif
		return r;
	}

	inline float4& operator *= (const float4& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_mul_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right[i];
		}
#endif
		return *this;
	}

	inline float4& operator /= (const float4& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_div_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right[i];
		}
#endif
		return *this;
	}

	inline float4& operator += (const float4& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_add_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right[i];
		}
#endif
		return *this;
	}

	inline float4& operator -= (const float4& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_sub_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right[i];
		}
#endif
		return *this;
	}

	inline float4 operator * (float right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_mul_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline float4 operator / (float right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_div_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline float4 operator + (float right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_add_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right;
		}
#endif
		return r;
	}

	inline float4 operator - (float right)
	{
		float4 r;
#if defined(USE_FAST_MATH)
		r.v = _mm_sub_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right;
		}
#endif
		return r;
	}

	inline float4& operator *= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_mul_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right;
		}
#endif
		return *this;
	}

	inline float4& operator /= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_div_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right;
		}
#endif
		return *this;
	}

	inline float4& operator += (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_add_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right;
		}
#endif
		return *this;
	}

	inline float4& operator -= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_sub_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right;
		}
#endif
		return *this;
	}
#pragma endregion

	inline float Length()
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	inline float4 Normalized()
	{
		float4 temp(*this);
		return temp / Length();
	}

	inline float4& Normalize()
	{
		*this = Normalized();
		return *this;
	}

	inline static float Dot(float4 left, float4 right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
	}
};


/*
----- Float3 -----
*/
struct float3
{
	union
	{
		float d[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float u, v, w; };
	};

	float3() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	float3(float X, float Y, float Z) : x(X), y(Y), z(Z)
	{
	}

	float3(const float3& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}

#pragma region OPERATORS

	inline float& operator[] (uint pos)
	{
		return d[pos];
	}

	inline float operator[] (uint pos) const
	{
		return (const float)d[pos];
	}

	inline float3 operator * (const float3& right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_mul_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
		r[2] = d.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right[i];
		}
#endif
		return r;
	}

	inline float3 operator / (const float3& right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_div_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
		r[2] = d.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right[i];
		}
#endif
		return r;
	}

	inline float3 operator + (const float3& right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_add_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
		r[2] = d.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right[i];
		}
#endif
		return r;
	}

	inline float3 operator - (const float3& right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_sub_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
		r[2] = d.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right[i];
		}
#endif
		return r;
	}

	inline float3& operator *= (const float3& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right[i];
		}
#endif
		return *this;
	}

	inline float3& operator /= (const float3& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right[i];
		}
#endif
		return *this;
	}

	inline float3& operator += (const float3& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right[i];
		}
#endif
		return *this;
	}

	inline float3& operator -= (const float3& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right.x, right.y, right.z, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right[i];
		}
#endif
		return *this;
	}

	inline float3 operator * (float right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
		r.z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right;
		}
#endif
		return r;
	}

	inline float3 operator / (float right)
	{
		float3 r;
#if defined(USE_FAST_MATH)		
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
		r.z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline float3 operator + (float right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
		r.z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right;
		}
#endif
		return r;
	}

	inline float3 operator - (float right)
	{
		float3 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
		r.z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right;
		}
#endif
		return r;
	}

	inline float3& operator *= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right;
		}
#endif
		return *this;
	}

	inline float3& operator /= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right;
		}
#endif
		return *this;
	}

	inline float3& operator += (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right;
		}
#endif
		return *this;
	}

	inline float3& operator -= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, z, 0.0f), _mm_set_ps(right, right, right, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];
#else
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right;
		}
#endif
		return *this;
	}

#pragma endregion

	inline float Length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float3 GetNormalized()
	{
		float3 temp(*this);
		return temp / Length();
	}

	inline float3& Normalize()
	{
		*this = GetNormalized();
		return *this;
	}

	static inline float3 Cross(float3 left, float3 right)
	{
		return float3(
			(left.y * right.z) - (left.z * right.y),
			(left.z * right.x) - (left.x * right.z),
			(left.x * right.y) - (left.y * right.x)
		);
	}

	static inline float Dot(float3 left, float3 right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z;
	}
};


/*
----- Float2 -----
*/
struct float2
{

	union
	{
		float d[2];
		struct { float x, y; };
		struct { float u, v; };
	};

	float2() : x(0.0f), y(0.0f)
	{
	}

	float2(float X, float Y) : x(X), y(Y)
	{
	}

	float2(const float2& other)
		: x(other.x), y(other.y)
	{
	}

#pragma region OPERATORS

	inline float& operator[] (uint pos)
	{
		return d[pos];
	}

	inline float operator[] (uint pos) const
	{
		return (const float)d[pos];
	}

	inline float2 operator * (const float2& right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_mul_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right[i];
		}
#endif
		return r;
	}

	inline float2 operator / (const float2& right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_div_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right[i];
		}
#endif
		return r;
	}

	inline float2 operator + (const float2& right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_add_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right[i];
		}
#endif
		return r;
	}

	inline float2 operator - (const float2& right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 d = _mm_sub_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		r[0] = d.m128_f32[0];
		r[1] = d.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right[i];
		}
#endif
		return r;
	}

	inline float2& operator *= (const float2& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right[i];
		}
#endif
		return *this;
	}

	inline float2& operator /= (const float2& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right[i];
		}
#endif
		return *this;
	}

	inline float2& operator += (const float2& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right[i];
		}
#endif
		return *this;
	}

	inline float2& operator -= (const float2& right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right.x, right.y, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right[i];
		}
#endif
		return *this;
	}

	inline float2 operator * (float right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right;
		}
#endif
		return r;
	}

	inline float2 operator / (float right)
	{
		float2 r;
#if defined(USE_FAST_MATH)		
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline float2 operator + (float right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right;
		}
#endif
		return r;
	}

	inline float2 operator - (float right)
	{
		float2 r;
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		r.x = v.m128_f32[0];
		r.y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right;
		}
#endif
		return r;
	}

	inline float2& operator *= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_mul_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right;
		}
#endif
		return *this;
	}

	inline float2& operator /= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_div_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right;
		}
#endif
		return *this;
	}

	inline float2& operator += (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_add_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right;
		}
#endif
		return *this;
	}

	inline float2& operator -= (float right)
	{
#if defined(USE_FAST_MATH)
		__m128 v = _mm_sub_ps(_mm_set_ps(x, y, 0.0f, 0.0f), _mm_set_ps(right, right, 0.0f, 0.0f));
		x = v.m128_f32[0];
		y = v.m128_f32[1];
#else
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right;
		}
#endif
		return *this;
	}

#pragma endregion

	inline float Length()
	{
		return sqrt(x * x + y * y);
	}

	inline float2 Normalized()
	{
		float2 temp(*this);
		return temp / Length();
	}

	inline float2& Normalize()
	{
		*this = Normalized();
		return *this;
	}

	static inline float Dot(float2 left, float2 right)
	{
		return left.x * right.x + left.y * right.y;
	}
};


/*
----- Quaternion -----
*/
struct quaternion
{
	union
	{
#if defined (USE_FAST_MATH)
		__m128 v;
#else
		float d[4];
#endif
		struct { float x, y, z, w; };
	};

	quaternion()
#if !defined(USE_FAST_MATH)
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
#else
	{
		v = _mm_set1_ps(0.0f);
	}
#endif
	quaternion(float X, float Y, float Z, float W)
#if !defined(USE_FAST_MATH)
		: x(X), y(Y), z(Z), w(W) { }
#else
	{
		v = _mm_set_ps(X, Y, Z, W);
	}
#endif

	quaternion(const quaternion& other)
#if !defined(USE_FAST_MATH)
		: x(other.x), y(other.y), z(other.z), w(other.w) { }
#else
	{
		_mm_store_ps(&v.m128_f32[0], other.v);
	}
#endif

#pragma region OPERATORS

#if defined(USE_FAST_MATH)
	inline operator __m128()
	{
		return this->v;
	}
#endif

#if defined(USE_FAST_MATH)
	inline operator __m128() const
	{
		return v;
	}
#endif

	inline quaternion& operator = (const quaternion& right)
	{
#if defined(USE_FAST_MATH)
		_mm_store_ps(&v.m128_f32[0], right.v);
#else
		for (uint i = 0; i < 4; i++)
			d[i] = right[i];
#endif
		return *this;
	}

	inline float& operator[] (uint pos)
	{
#if defined(USE_FAST_MATH)
		return v.m128_f32[pos];
#else
		return d[pos];
#endif
	}

	inline float operator[] (uint pos) const
	{
#if defined(USE_FAST_MATH)
		return (const float)v.m128_f32[pos];
#else
		return (const float)d[pos];
#endif
	}

	inline quaternion operator * (const quaternion& right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_mul_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] * right[i];
		}
#endif
		return r;
	}

	inline quaternion operator / (const quaternion& right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_div_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right[i];
		}
#endif
		return r;
	}

	inline quaternion operator + (const quaternion& right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_add_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right[i];
		}
#endif
		return r;
	}

	inline quaternion operator - (const quaternion& right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_sub_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right[i];
		}
#endif
		return r;
	}

	inline quaternion& operator *= (const quaternion& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_mul_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right[i];
		}
#endif
		return *this;
	}

	inline quaternion& operator /= (const quaternion& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_div_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right[i];
		}
#endif
		return *this;
	}

	inline quaternion& operator += (const quaternion& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_add_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right[i];
		}
#endif
		return *this;
	}

	inline quaternion& operator -= (const quaternion& right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_sub_ps(v, right.v);
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right[i];
		}
#endif
		return *this;
	}

	inline quaternion operator * (float right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_mul_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline quaternion operator / (float right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_div_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
#endif
		return r;
	}

	inline quaternion operator + (float right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_add_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right;
		}
#endif
		return r;
	}

	inline quaternion operator - (float right)
	{
		quaternion r;
#if defined(USE_FAST_MATH)
		r.v = _mm_sub_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right;
		}
#endif
		return r;
	}

	inline quaternion& operator *= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_mul_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right;
		}
#endif
		return *this;
	}

	inline quaternion& operator /= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_div_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right;
		}
#endif
		return *this;
	}

	inline quaternion& operator += (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_add_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right;
		}
#endif
		return *this;
	}

	inline quaternion& operator -= (float right)
	{
#if defined(USE_FAST_MATH)
		v = _mm_sub_ps(v, _mm_set1_ps(right));
#else
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right;
		}
#endif
		return *this;
	}
#pragma endregion

	inline float Length()
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	inline quaternion Normalized()
	{
		quaternion temp(*this);
		return temp / Length();
	}

	inline quaternion& Normalize()
	{
		*this = Normalized();
		return *this;
	}

	static inline quaternion Euler(const float3& EulerAngles)
	{
		return quaternion();
	}
};

inline float ToDegrees(float radians)
{
	return radians * (_PI_ / 180.f);
}

inline float ToRadians(float degrees)
{
	return degrees * (180.f / _PI_);
}


struct matrix
{
	union
	{
		float4 fs[4];
#if defined(USE_FAST_MATH)
		__m128 pf[4];
#endif
		float af[16];
		float ff[4][4];
	};

	matrix()
	{
		for (uint i = 0; i < 4; i++)
		{
			fs[i] = float4();
		}
	}

	matrix(const float4& r1, const float4& r2, const float4& r3, const float4& r4)
	{
		fs[0] = r1;
		fs[1] = r2;
		fs[2] = r3;
		fs[3] = r4;
	}

	matrix( float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33 )
	{
		ff[0][0] = m00; ff[0][1] = m01, ff[0][2] = m02; ff[0][3] = m03;
		ff[1][0] = m10; ff[1][1] = m11, ff[1][2] = m12; ff[1][3] = m13;
		ff[2][0] = m20; ff[2][1] = m21, ff[2][2] = m22; ff[2][3] = m23;
		ff[3][0] = m30; ff[3][1] = m31, ff[3][2] = m32; ff[3][3] = m33;
	}

	matrix(const matrix& mat)
	{
		for (uint i = 0; i < 4; i++)
		{
			fs[i] = mat.fs[i];
		}
	}


	inline float4& operator [] (int row)
	{
		return fs[row];
	}

	inline float4 operator [] (int row) const
	{
		return (const float4)fs[row];
	}


	inline matrix& operator = (const matrix& mat)
	{
		for (uint i = 0; i < 4; i++)
		{
			fs[i] = mat.fs[i];
		}

		return *this;
	}

	inline matrix operator * (const matrix& right)
	{
		matrix ret(*this);

		for (uint i = 0; i < 4; i++)
		{
			const float x = ret.ff[i][0];
			const float y = ret.ff[i][1];
			const float z = ret.ff[i][2];
			const float w = ret.ff[i][3];

			ret.ff[i][0] = (right[0][0] * x) + (right[1][0] * y) + (right[2][0] * z) + (right[3][0] * w);
			ret.ff[i][1] = (right[0][1] * x) + (right[1][1] * y) + (right[2][1] * z) + (right[3][1] * w);
			ret.ff[i][2] = (right[0][2] * x) + (right[1][2] * y) + (right[2][2] * z) + (right[3][2] * w);
			ret.ff[i][3] = (right[0][3] * x) + (right[1][3] * y) + (right[2][3] * z) + (right[3][3] * w);
		}

		return ret;
	}

	inline matrix& operator *= (const matrix& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			const float x = ff[i][0];
			const float y = ff[i][1];
			const float z = ff[i][2];
			const float w = ff[i][3];

			ff[i][0] = (right[0][0] * x) + (right[1][0] * y) + (right[2][0] * z) + (right[3][0] * w);
			ff[i][1] = (right[0][1] * x) + (right[1][1] * y) + (right[2][1] * z) + (right[3][1] * w);
			ff[i][2] = (right[0][2] * x) + (right[1][2] * y) + (right[2][2] * z) + (right[3][2] * w);
			ff[i][3] = (right[0][3] * x) + (right[1][3] * y) + (right[2][3] * z) + (right[3][3] * w);
		}

		return *this;
	}

	inline float4 operator * (const float4& right)
	{
		float4 Ret;

		Ret.x = (ff[0][0] * right.x) + (ff[0][1] * right.y) + (ff[0][2] * right.z) + (ff[0][3] * right.w);
		Ret.y = (ff[1][0] * right.x) + (ff[1][1] * right.y) + (ff[1][2] * right.z) + (ff[1][3] * right.w);
		Ret.z = (ff[2][0] * right.x) + (ff[2][1] * right.y) + (ff[2][2] * right.z) + (ff[2][3] * right.w);
		Ret.w = (ff[3][0] * right.x) + (ff[3][1] * right.y) + (ff[3][2] * right.z) + (ff[3][3] * right.w);

		return Ret;
	}

	inline matrix operator + (const matrix& right)
	{
		matrix ret(*this);

		for (uint i = 0; i < 4; i++)
			ret[i] += right[i];

		return ret;
	}

	inline matrix& operator += (const matrix& right)
	{
		for (uint i = 0; i < 4; i++)
			fs[i] += right[i];

		return *this;
	}

	inline matrix& SetIdentity()
	{
		for (uint i = 0; i < 4; i++)
		{
			fs[i] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		ff[0][0] = 1.0f;
		ff[1][1] = 1.0f;
		ff[2][2] = 1.0f;
		ff[3][3] = 1.0f;
		return *this;
	}

	static inline matrix Perspective(float fovY, float aspect, float zNear, float zFar)
	{
		float tanFovOver2 = tan(ToRadians(fovY / 2));
		float range = zFar - zNear;

		matrix ret;
		ret.SetIdentity();
		ret[0][0] = 1 / (aspect * tanFovOver2);
		ret[1][1] = 1 / tanFovOver2;
		ret[2][2] = -((zFar + zNear) / (range));
		ret[3][2] = -1.0f;
		ret[2][3] = -((2 * zFar * zNear) / (range));

		return ret;
	}

	static inline matrix Transpose(const matrix inMat)
	{
		matrix ret;
		for (uint x = 0; x < 4; x++)
		{
			for (uint y = 0; y < 4; y++)
			{
				ret[x][y] = inMat[y][x];
			}
		}
		return ret;
	}

	static inline matrix LookAt(float3 eye, float3 target, float3 up)
	{
		float3 zaxis = (eye - target).GetNormalized();				// The "forward" vector.
		float3 xaxis = float3::Cross(up, zaxis).GetNormalized();	// The "right" vector.
		float3 yaxis = float3::Cross(zaxis, xaxis);				// The "up" vector.

		matrix viewMatrix;
		viewMatrix[0] = float4(xaxis.x, xaxis.y, xaxis.z, -float3::Dot(xaxis, eye));
		viewMatrix[1] = float4(yaxis.x, yaxis.y, yaxis.z, -float3::Dot(yaxis, eye));
		viewMatrix[2] = float4(zaxis.x, zaxis.y, zaxis.z, -float3::Dot(zaxis, eye));
		viewMatrix[3] = float4(0, 0, 0, 1);
		return viewMatrix;
	}

	static inline matrix FromQuaternion(quaternion inQuat)
	{

	}

	static inline matrix RotateX(float angle)
	{
		float sinOfAngle = sin(angle);
		float cosOfAngle = cos(angle);

		matrix Ret;
		Ret.SetIdentity();

		Ret[1][1] = cosOfAngle;
		Ret[2][2] = cosOfAngle;
		Ret[1][2] = -sinOfAngle;
		Ret[2][1] = sinOfAngle;

		return Ret;
	}

	static inline matrix RotateY(float angle)
	{
		float sinOfAngle = sin(angle);
		float cosOfAngle = cos(angle);

		matrix Ret;
		Ret.SetIdentity();

		Ret[0][0] = cosOfAngle;
		Ret[0][2] = sinOfAngle;
		Ret[2][0] = -sinOfAngle;
		Ret[2][2] = cosOfAngle;

		return Ret;
	}

	static inline matrix RotateZ(float angle)
	{
		float sinOfAngle = sin(angle);
		float cosOfAngle = cos(angle);

		matrix Ret;
		Ret.SetIdentity();

		Ret[0][0] = cosOfAngle;
		Ret[0][1] = -sinOfAngle;
		Ret[1][0] = sinOfAngle;
		Ret[1][1] = cosOfAngle;

		return Ret;
	}

	static inline matrix Translate(float3 newPos)
	{
		matrix ret;
		ret.SetIdentity();

		ret[0][3] = newPos.x;
		ret[1][3] = newPos.y;
		ret[2][3] = newPos.z;

		return ret;
	}

	static inline matrix Identity()
	{
		matrix ret;
		ret.SetIdentity();
		return ret;
	}
};

typedef matrix float4x4;


/*
----- Int4 -----
*/
struct int4
{
	union
	{
		int d[4];
		struct { int x, y, z, w; };
		struct { int r, g, b, a; };
	};

	int4() : x(0), y(0), z(0), w(0) { }
	int4(int X, int Y, int Z, int W) : x(X), y(Y), z(Z), w(W) { }

	int4(const int4& other) : x(other.x), y(other.y), z(other.z), w(other.w) { }

#pragma region OPERATORS


	inline int4& operator = (const int4& right)
	{
		for (uint i = 0; i < 4; i++)
			d[i] = right[i];
		return *this;
	}

	inline int& operator[] (uint pos)
	{
		return d[pos];
	}

	inline int operator[] (uint pos) const
	{
		return (const int)d[pos];
	}

	inline int4 operator * (const int4& right)
	{
		int4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] * right[i];
		}

		return r;
	}

	inline int4 operator / (const int4& right)
	{
		int4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right[i];
		}

		return r;
	}

	inline int4 operator + (const int4& right)
	{
		int4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right[i];
		}

		return r;
	}

	inline int4 operator - (const int4& right)
	{
		int4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right[i];
		}

		return r;
	}

	inline int4& operator *= (const int4& right)
	{

		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right[i];
		}

		return *this;
	}

	inline int4& operator /= (const int4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline int4& operator += (const int4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline int4& operator -= (const int4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline int4 operator * (int right)
	{
		int4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline int4 operator / (int right)
	{
		int4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline int4 operator + (int right)
	{
		int4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline int4 operator - (int right)
	{
		int4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline int4& operator *= (int right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline int4& operator /= (int right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline int4& operator += (int right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline int4& operator -= (int right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right;
		}
		return *this;
	}
#pragma endregion

};


/*
----- Int3 -----
*/
struct int3
{
	union
	{
		int d[3];
		struct { int x, y, z; };
		struct { int r, g, b; };
		struct { int u, v, w; };
	};

	int3() : x(0), y(0), z(0)
	{
	}

	int3(int X, int Y, int Z) : x(X), y(Y), z(Z)
	{
	}

	int3(const int3& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}

#pragma region OPERATORS

	inline int& operator[] (uint pos)
	{
		return d[pos];
	}

	inline int operator[] (uint pos) const
	{
		return (const int)d[pos];
	}

	inline int3 operator * (const int3& right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right[i];
		}
		return r;
	}

	inline int3 operator / (const int3& right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right[i];
		}
		return r;
	}

	inline int3 operator + (const int3& right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right[i];
		}
		return r;
	}

	inline int3 operator - (const int3& right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right[i];
		}
		return r;
	}

	inline int3& operator *= (const int3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right[i];
		}
		return *this;
	}

	inline int3& operator /= (const int3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline int3& operator += (const int3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline int3& operator -= (const int3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline int3 operator * (int right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right;
		}
		return r;
	}

	inline int3 operator / (int right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline int3 operator + (int right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline int3 operator - (int right)
	{
		int3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline int3& operator *= (int right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline int3& operator /= (int right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline int3& operator += (int right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline int3& operator -= (int right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right;
		}
		return *this;
	}

#pragma endregion

};


/*
----- Int2 -----
*/
struct int2
{
	union
	{
		int d[2];
		struct { int x, y; };
		struct { int u, v; };
	};

	int2() : x(0), y(0)
	{
	}

	int2(int X, int Y) : x(X), y(Y)
	{
	}

	int2(const int2& other)
		: x(other.x), y(other.y)
	{
	}

#pragma region OPERATORS

	inline int& operator[] (uint pos)
	{
		return d[pos];
	}

	inline int operator[] (uint pos) const
	{
		return (const int)d[pos];
	}

	inline int2 operator * (const int2& right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right[i];
		}
		return r;
	}

	inline int2 operator / (const int2& right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right[i];
		}
		return r;
	}

	inline int2 operator + (const int2& right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right[i];
		}
		return r;
	}

	inline int2 operator - (const int2& right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right[i];
		}
		return r;
	}

	inline int2& operator *= (const int2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right[i];
		}
		return *this;
	}

	inline int2& operator /= (const int2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline int2& operator += (const int2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline int2& operator -= (const int2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline int2 operator * (int right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right;
		}
		return r;
	}

	inline int2 operator / (int right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline int2 operator + (int right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline int2 operator - (int right)
	{
		int2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline int2& operator *= (int right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline int2& operator /= (int right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline int2& operator += (int right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline int2& operator -= (int right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right;
		}
		return *this;
	}

#pragma endregion

};



/*
----- Int4 -----
*/
struct uint4
{
	union
	{
		uint d[4];
		struct { uint x, y, z, w; };
		struct { uint r, g, b, a; };
	};

	uint4() : x(0), y(0), z(0), w(0) { }
	uint4(uint X, uint Y, uint Z, uint W) : x(X), y(Y), z(Z), w(W) { }

	uint4(const uint4& other) : x(other.x), y(other.y), z(other.z), w(other.w) { }

#pragma region OPERATORS


	inline uint4& operator = (const uint4& right)
	{
		for (uint i = 0; i < 4; i++)
			d[i] = right[i];
		return *this;
	}

	inline uint& operator[] (uint pos)
	{
		return d[pos];
	}

	inline uint operator[] (uint pos) const
	{
		return (const uint)d[pos];
	}

	inline uint4 operator * (const uint4& right)
	{
		uint4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] * right[i];
		}

		return r;
	}

	inline uint4 operator / (const uint4& right)
	{
		uint4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right[i];
		}

		return r;
	}

	inline uint4 operator + (const uint4& right)
	{
		uint4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right[i];
		}

		return r;
	}

	inline uint4 operator - (const uint4& right)
	{
		uint4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right[i];
		}

		return r;
	}

	inline uint4& operator *= (const uint4& right)
	{

		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right[i];
		}

		return *this;
	}

	inline uint4& operator /= (const uint4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline uint4& operator += (const uint4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline uint4& operator -= (const uint4& right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline uint4 operator * (uint right)
	{
		uint4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline uint4 operator / (uint right)
	{
		uint4 r;

		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline uint4 operator + (uint right)
	{
		uint4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline uint4 operator - (uint right)
	{
		uint4 r;
		for (uint i = 0; i < 4; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline uint4& operator *= (uint right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline uint4& operator /= (uint right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline uint4& operator += (uint right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline uint4& operator -= (uint right)
	{
		for (uint i = 0; i < 4; i++)
		{
			d[i] -= right;
		}
		return *this;
	}
#pragma endregion

};


/*
----- uint3 -----
*/
struct uint3
{
	union
	{
		uint d[3];
		struct { uint x, y, z; };
		struct { uint r, g, b; };
		struct { uint u, v, w; };
	};

	uint3() : x(0), y(0), z(0)
	{
	}

	uint3(uint X, uint Y, uint Z) : x(X), y(Y), z(Z)
	{
	}

	uint3(const uint3& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}

#pragma region OPERATORS

	inline uint& operator[] (uint pos)
	{
		return d[pos];
	}

	inline uint operator[] (uint pos) const
	{
		return (const uint)d[pos];
	}

	inline uint3 operator * (const uint3& right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right[i];
		}
		return r;
	}

	inline uint3 operator / (const uint3& right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right[i];
		}
		return r;
	}

	inline uint3 operator + (const uint3& right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right[i];
		}
		return r;
	}

	inline uint3 operator - (const uint3& right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right[i];
		}
		return r;
	}

	inline uint3& operator *= (const uint3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right[i];
		}
		return *this;
	}

	inline uint3& operator /= (const uint3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline uint3& operator += (const uint3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline uint3& operator -= (const uint3& right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline uint3 operator * (uint right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] * right;
		}
		return r;
	}

	inline uint3 operator / (uint right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline uint3 operator + (uint right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline uint3 operator - (uint right)
	{
		uint3 r;
		for (uint i = 0; i < 3; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline uint3& operator *= (uint right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline uint3& operator /= (uint right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline uint3& operator += (uint right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline uint3& operator -= (uint right)
	{
		for (uint i = 0; i < 3; i++)
		{
			d[i] -= right;
		}
		return *this;
	}

#pragma endregion

};


/*
----- uint2 -----
*/
struct uint2
{
	union
	{
		uint d[2];
		struct { uint x, y; };
		struct { uint u, v; };
	};

	uint2() : x(0), y(0)
	{
	}

	uint2(uint X, uint Y) : x(X), y(Y)
	{
	}

	uint2(const uint2& other)
		: x(other.x), y(other.y)
	{
	}

#pragma region OPERATORS

	inline uint& operator[] (uint pos)
	{
		return d[pos];
	}

	inline uint operator[] (uint pos) const
	{
		return (const uint)d[pos];
	}

	inline uint2 operator * (const uint2& right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right[i];
		}
		return r;
	}

	inline uint2 operator / (const uint2& right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right[i];
		}
		return r;
	}

	inline uint2 operator + (const uint2& right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right[i];
		}
		return r;
	}

	inline uint2 operator - (const uint2& right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right[i];
		}
		return r;
	}

	inline uint2& operator *= (const uint2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right[i];
		}
		return *this;
	}

	inline uint2& operator /= (const uint2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right[i];
		}
		return *this;
	}

	inline uint2& operator += (const uint2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right[i];
		}
		return *this;
	}

	inline uint2& operator -= (const uint2& right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right[i];
		}
		return *this;
	}

	inline uint2 operator * (uint right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] * right;
		}
		return r;
	}

	inline uint2 operator / (uint right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] / right;
		}
		return r;
	}

	inline uint2 operator + (uint right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] + right;
		}
		return r;
	}

	inline uint2 operator - (uint right)
	{
		uint2 r;
		for (uint i = 0; i < 2; i++)
		{
			r[i] = d[i] - right;
		}
		return r;
	}

	inline uint2& operator *= (uint right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] *= right;
		}
		return *this;
	}

	inline uint2& operator /= (uint right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] /= right;
		}
		return *this;
	}

	inline uint2& operator += (uint right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] += right;
		}
		return *this;
	}

	inline uint2& operator -= (uint right)
	{
		for (uint i = 0; i < 2; i++)
		{
			d[i] -= right;
		}
		return *this;
	}

#pragma endregion

};
#endif

#endif //__BANE_MATH_H__