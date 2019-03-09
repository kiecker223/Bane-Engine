#pragma once

#include "4RowColMatrix.h"
#include "MathFunctions.h"


struct Quaternion
{
public:

	union 
	{
		__m128 f;
		struct { float x, y, z, w; };
	};


	Quaternion()
	{
		f = { };
	}

	Quaternion(const Quaternion& Rhs)
	{
		f = Rhs.f;
	}

	Quaternion(float W, float X, float Y, float Z) 
	{
		f = _mm_set_ps(W, X, Y, Z);
	}

	Quaternion(const fvec3& Euler)
	{
		*this = ((Quaternion(fvec3(1.f, 0.f, 0.f), radians(Euler.x)) * Quaternion(fvec3(0.f, 1.f, 0.f), radians(Euler.y)) * Quaternion(fvec3(0.f, 0.f, 1.f), radians(Euler.z))).Normalized());
	}

	Quaternion(const fvec3& Axis, const float Angle)
	{
		*this = Quaternion::FromAxisAngle(Axis, Angle);
	}

	inline static Quaternion Identity() 
	{
		return Quaternion(1.f, 0.f, 0.f, 0.f);
	}

	inline Quaternion& operator = (const Quaternion& Rhs)
	{
		f = Rhs.f;
		return *this;
	}

	inline static Quaternion FromAxisAngle(const fvec3& InAxis, const float InAngle)
	{
		fvec4 Axis(InAxis, 0.0f);
		float Angle = InAngle;
		if (InAngle > 6.28318531f)
		{
			Angle = fmod(Angle, 6.28318531f);
		}
		Quaternion Result;
		const float s = sinf(Angle / 2.f);
		__m128 S = _mm_load_ps1(&s);
		__m128 A = Axis.f;
		Result.f = _mm_mul_ps(S, A);
		Result.w = cosf(Angle / 2.f);
		Result.Normalize();
		return Result;
	}

	inline static Quaternion FromAxisAngle(const vec3& InAxis, const float InAngle)
	{
		return FromAxisAngle(fvec3(static_cast<float>(InAxis.x), static_cast<float>(InAxis.y), static_cast<float>(InAxis.z)), InAngle);
	}

	inline static Quaternion FromAxisAngle(const vec3& InAxis, const double InAngle)
	{
		return FromAxisAngle(InAxis, static_cast<float>(InAngle));
	}

	inline static Quaternion FromDirection(const fvec3& Direction)
	{
		float Angle = atan2f(Direction.x, Direction.z);
		return Quaternion(0.0f, sinf(Angle / 2.f), 0.f, cosf(Angle / 2.f));
	}

	inline Quaternion Conjugate()
	{
		return Quaternion(-x, -y, -z, w);
	}

	// Credit to: https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
	inline fvec3 operator * (const fvec3& Rhs) const
	{
		fvec3 u(x, y, z);

		float s = w;

		return (u * (2.0f * dot(u, Rhs)) + (Rhs * (s*s - dot(u, u))) + (cross(u, Rhs) * 2.0f) * s);
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

	inline fvec3 GetForward() const
	{
		return fvec3(
			2.f * x * z - 2.f * w * y,
			2.f * y * z + 2.f * w * x,
			1.f - 2.f * y * y - 2.f * x * x
		);
	}

	inline fvec3 GetBack() const
	{
		return -GetForward();
	}

	inline fvec3 GetUp() const
	{
		return fvec3(
			2.f * x * y * 2.f * w * z, 
			1.f - 2.f * z * z - 2.f * x * x,
			2.f * z * y - 2.f * x * w
		);
	}

	inline fvec3 GetDown() const
	{
		return -GetUp();
	}

	inline fvec3 GetRight() const
	{
		return fvec3(
			1.f - 2.f *z *z - 2.f * y * y,
			-2.f * z * w + 2 * y * x,
			2 * y * w + 2 * z * x
		);
	}

	inline fvec3 GetLeft() const
	{
		return -GetRight();
	}

	inline matrix RotationMatrix() const
	{
		const fvec3 R = GetRight();
		const fvec3 U = GetUp();
		const fvec3 F = GetForward();
		matrix Result(
			fvec4(R, 0.f),
			fvec4(U, 0.f),
			fvec4(F, 0.f),
			fvec4(0.f, 0.f, 0.f, 1.f)
		);
		return Result;
	}

	inline mat4x4 RotationMatrixDoubles() const
	{
		const vec3 R = fromFloat3(GetRight());
		const vec3 U = fromFloat3(GetUp());
		const vec3 F = fromFloat3(GetForward());
		return mat4x4(
			vec4(R, 0.f),
			vec4(U, 0.f),
			vec4(F, 0.f),
			vec4(0.f, 0.f, 0.f, 0.f)
		);
	}

	inline fmat3x3 RotationMatrix3x3() const
	{
		return fmat3x3(
			GetRight(),
			GetUp(),
			GetForward()
		);
	}

	inline mat3x3 RotationMatrix3x3Doubles() const
	{
		return mat3x3(
			fromFloat3(GetRight()),
			fromFloat3(GetUp()),
			fromFloat3(GetForward())
		);
	}

	inline Quaternion Normalized() const
	{
		Quaternion q(*this);
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		__m128 L = _mm_load_ps1(&Length);
		q.f = _mm_div_ps(f, L);
		return q;
	}

	inline Quaternion& Normalize()
	{
		float Length = sqrtf(w * w + x * x + y * y + z * z);
		__m128 L = _mm_load_ps1(&Length);
		f = _mm_div_ps(f, L);
		return *this;
	}
};

struct Quaternion64
{
public:

	double w, x, y, z;

	Quaternion64()
	{
	}

	Quaternion64(const Quaternion64& Rhs) :
		w(Rhs.w),
		x(Rhs.x),
		y(Rhs.y),
		z(Rhs.z)
	{
	}

	Quaternion64(double W, double X, double Y, double Z) :
		w(W),
		x(X),
		y(Y),
		z(Z)
	{
	}

	Quaternion64(const vec3& Euler)
	{
		*this = ((Quaternion64(vec3(1., 0., 0.), radiansD(Euler.x)) * Quaternion64(vec3(0., 1., 0.), radiansD(Euler.y)) * Quaternion64(vec3(0., 0., 1.), radiansD(Euler.z))).Normalized());
	}

	Quaternion64(const vec3& Axis, const double Angle)
	{
		*this = Quaternion64::FromAxisAngle(Axis, Angle);
	}

	inline Quaternion64& operator = (const Quaternion64& Rhs)
	{
		w = Rhs.w;
		x = Rhs.x;
		y = Rhs.y;
		z = Rhs.z;
		return *this;
	}

	inline static Quaternion64 FromAxisAngle(const vec3& InAxis, const double Angle)
	{
		vec3 Axis = InAxis;
		Quaternion64 Result;
		const double s = sin(Angle / 2);
		Result.x = Axis.x * s;
		Result.y = Axis.y * s;
		Result.z = Axis.z * s;
		Result.w = cos(Angle / 2);
		Result.Normalize(); // The length is always 1
		return Result;
	}

	inline static Quaternion64 FromDirection(const vec3& Direction)
	{
		double Angle = atan2(Direction.x, Direction.z);
		return Quaternion64(0.0f, sin(Angle / 2.f), 0.f, cos(Angle / 2.f));
	}

	inline Quaternion64 Conjugate()
	{
		return Quaternion64(-x, -y, -z, w);
	}

	inline Quaternion64 operator * (const Quaternion64& Rhs) const
	{
		Quaternion64 Result;

		Result.w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		Result.x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		Result.y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		Result.z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);

		return Result;
	}

	inline Quaternion64& operator *= (const Quaternion64& Rhs)
	{
		w = (w * Rhs.w - x * Rhs.x - y * Rhs.y - z * Rhs.z);
		x = (w * Rhs.x + x * Rhs.w - y * Rhs.z + z * Rhs.y);
		y = (w * Rhs.y + x * Rhs.z + y * Rhs.w - z * Rhs.x);
		z = (w * Rhs.z - x * Rhs.y + y * Rhs.x + z * Rhs.w);
		return *this;
	}

	inline vec3 GetForward() const
	{
		return vec3(2.f * (x * z - w * y), 2.f * (y * z + w * z), 1.f - 2.f * (x * x + y * y));
	}

	inline vec3 GetBack() const
	{
		return -GetForward();
	}

	inline vec3 GetUp() const
	{
		return vec3(2.f * (x * y + w * z), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - w * x));
	}

	inline vec3 GetDown() const
	{
		return -GetUp();
	}

	inline vec3 GetRight() const
	{
		return vec3(1.f - 2.f * (y * y + z * z), 2.f * (x * y - w * z), 2.f * (x * z + w * y));
	}

	inline vec3 GetLeft() const
	{
		return -GetRight();
	}

	inline mat4x4 RotationMatrix() const
	{
		const vec3 R = GetRight();
		const vec3 U = GetUp();
		const vec3 F = GetForward();
		mat4x4 Result(
			vec4(R, 0.),
			vec4(U, 0.),
			vec4(F, 0.),
			vec4(0., 0., 0., 1.)
		);
		return Result;
	}

	inline Quaternion64 Normalized() const
	{
		Quaternion64 q(*this);
		double Length = sqrt(w * w + x * x + y * y + z * z);
		q.w /= Length;
		q.x /= Length;
		q.y /= Length;
		q.z /= Length;
		return q;
	}

	inline Quaternion64& Normalize()
	{
		double Length = sqrt(w * w + x * x + y * y + z * z);
		w /= Length;
		x /= Length;
		y /= Length;
		z /= Length;
		return *this;
	}
};

inline matrix matRotation(const Quaternion& Rotation)
{
	return Rotation.RotationMatrix();
}



inline matrix matTransformation(const fvec3& Position, const Quaternion& Rotation, const fvec3& Scale)
{
	return matTranslation(Position) * matRotation(Rotation) * matScale(Scale);
}



inline mat4x4 matRotation(const Quaternion64& Rotation)
{
	return Rotation.RotationMatrix();
}

inline mat4x4 matTransformation(const vec3& Position, const Quaternion64& Rotation, const vec3& Scale)
{
	mat4x4 Result;

	const mat4x4 m1 = matTranslation(Position);
	const mat4x4 m2 = matRotation(Rotation);
	const mat4x4 m3 = matScale(Scale);

	Result = m1 * m2 * m3;

	return Result;
}


inline Quaternion FromQuat64(const Quaternion64& InVal)
{
	return Quaternion(static_cast<float>(InVal.w), static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z));
}