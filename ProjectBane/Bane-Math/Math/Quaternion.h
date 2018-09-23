#pragma once

#include "4RowColMatrix.h"

#define _PI_ 3.141592654f
#define _M_PI_ 3.1415926535897932384

inline float radians(float Degrees)
{
	return (_PI_ / 180.f) * Degrees;
}

inline float degrees(float Radians)
{
	return (180.f / _PI_) * Radians;
}

inline double radiansD(double Degrees)
{
	return (_M_PI_ / 180.) * Degrees;
}

inline double degreesD(double Radians)
{
	return (180. / _M_PI_) * Radians;
}

struct Quaternion
{
public:

	float w, x, y, z;

	Quaternion()
	{
	}

	Quaternion(const Quaternion& Rhs) :
		w(Rhs.w),
		x(Rhs.x),
		y(Rhs.y),
		z(Rhs.z)
	{
	}

	Quaternion(float W, float X, float Y, float Z) :
		w(W),
		x(X),
		y(Y),
		z(Z)
	{
	}

	Quaternion(const float3& Euler)
	{
		*this = ((Quaternion(float3(1.f, 0.f, 0.f), radians(Euler.x)) * Quaternion(float3(0.f, 1.f, 0.f), radians(Euler.y)) * Quaternion(float3(0.f, 0.f, 1.f), radians(Euler.z))).Normalized());
	}

	Quaternion(const float3& Axis, const float Angle)
	{
		*this = Quaternion::FromAxisAngle(Axis, Angle);
	}

	inline Quaternion& operator = (const Quaternion& Rhs)
	{
		w = Rhs.w;
		x = Rhs.x;
		y = Rhs.y;
		z = Rhs.z;
		return *this;
	}

	inline static Quaternion FromAxisAngle(const float3& InAxis, const float Angle)
	{
		float3 Axis = InAxis;
		Quaternion Result;
		const float s = sinf(Angle / 2);
		Result.x = Axis.x * s;
		Result.y = Axis.y * s;
		Result.z = Axis.z * s;
		Result.w = cosf(Angle / 2);
		Result.Normalize(); // The length is always 1
		return Result;
	}

	inline Quaternion Conjugate()
	{
		return Quaternion(-x, -y, -z, w);
	}

	inline Quaternion operator * (const Quaternion& Rhs) const
	{
		Quaternion Result;

		Result.w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		Result.x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		Result.y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		Result.z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);

		return Result;
	}

	inline Quaternion& operator *= (const Quaternion& Rhs)
	{
		w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);
		return *this;
	}

	inline float3 GetForward() const
	{
		return float3(2.f * (x * z - w * y), 2.f * (y * z + w * z), 1.f - 2.f * (x * x + y * y));
	}

	inline float3 GetBack() const
	{
		return -GetForward();
	}

	inline float3 GetUp() const
	{
		return float3(2.f * (x * y + w * z), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - w * x));
	}

	inline float3 GetDown() const
	{
		return -GetUp();
	}

	inline float3 GetRight() const
	{
		return float3(1.f - 2.f * (y * y + z * z), 2.f * (x * y - w * z), 2.f * (x * z + w * y));
	}

	inline float3 GetLeft() const
	{
		return -GetRight();
	}

	inline matrix RotationMatrix() const
	{
		const float3 R = GetRight();
		const float3 U = GetUp();
		const float3 F = GetForward();
		matrix Result(
			float4(R, 0.f),
			float4(U, 0.f),
			float4(F, 0.f),
			float4(0.f, 0.f, 0.f, 1.f)
		);
		return Result;
	}

	inline Quaternion Normalized() const
	{
		Quaternion q(*this);
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		q.w /= Length;
		q.x /= Length;
		q.y /= Length;
		q.z /= Length;
		return q;
	}

	inline Quaternion& Normalize()
	{
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		w /= Length;
		x /= Length;
		y /= Length;
		z /= Length;
		return *this;
	}
};

/* struct Quaternion
{
public:

	float w, x, y, z;

	Quaternion()
	{
	}

	Quaternion(const Quaternion& Rhs) :
		w(Rhs.w),
		x(Rhs.x),
		y(Rhs.y),
		z(Rhs.z)
	{
	}

	Quaternion(float W, float X, float Y, float Z) :
		w(W),
		x(X),
		y(Y),
		z(Z)
	{
	}

	Quaternion(const float3& Euler)
	{
		*this = ((Quaternion(float3(1.f, 0.f, 0.f), radians(Euler.x)) * Quaternion(float3(0.f, 1.f, 0.f), radians(Euler.y)) * Quaternion(float3(0.f, 0.f, 1.f), radians(Euler.z))).Normalized());
	}

	Quaternion(const float3& Axis, const float Angle)
	{
		*this = Quaternion::FromAxisAngle(Axis, Angle);
	}

	inline Quaternion& operator = (const Quaternion& Rhs)
	{
		w = Rhs.w;
		x = Rhs.x;
		y = Rhs.y;
		z = Rhs.z;
		return *this;
	}

	inline static Quaternion FromAxisAngle(const float3& InAxis, const float Angle)
	{
		float3 Axis = InAxis;
		Quaternion Result;
		const float s = sinf(Angle / 2);
		Result.x = Axis.x * s;
		Result.y = Axis.y * s;
		Result.z = Axis.z * s;
		Result.w = cosf(Angle / 2);
		Result.Normalize(); // The length is always 1
		return Result;
	}

	inline Quaternion Conjugate()
	{
		return Quaternion(-x, -y, -z, w);
	}

	inline Quaternion operator * (const Quaternion& Rhs) const
	{
		Quaternion Result;

		Result.w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		Result.x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		Result.y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		Result.z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);

		return Result;
	}

	inline Quaternion& operator *= (const Quaternion& Rhs)
	{
		w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);
		return *this;
	}

	inline float3 GetForward() const
	{
		return float3(2.f * (x * z - w * y), 2.f * (y * z + w * z), 1.f - 2.f * (x * x + y * y));
	}

	inline float3 GetBack() const
	{
		return -GetForward();
	}

	inline float3 GetUp() const
	{
		return float3(2.f * (x * y + w * z), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - w * x));
	}

	inline float3 GetDown() const
	{
		return -GetUp();
	}

	inline float3 GetRight() const
	{
		return float3(1.f - 2.f * (y * y + z * z), 2.f * (x * y - w * z), 2.f * (x * z + w * y));
	}

	inline float3 GetLeft() const
	{
		return -GetRight();
	}

	inline matrix RotationMatrix() const
	{
		const float3 R = GetRight();
		const float3 U = GetUp();
		const float3 F = GetForward();
		matrix Result(
			float4(R, 0.f),
			float4(U, 0.f),
			float4(F, 0.f),
			float4(0.f, 0.f, 0.f, 1.f)
		);
		return Result;
	}

	inline Quaternion Normalized() const
	{
		Quaternion q(*this);
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		q.w /= Length;
		q.x /= Length;
		q.y /= Length;
		q.z /= Length;
		return q;
	}

	inline Quaternion& Normalize()
	{
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		w /= Length;
		x /= Length;
		y /= Length;
		z /= Length;
		return *this;
	}
};

*/
