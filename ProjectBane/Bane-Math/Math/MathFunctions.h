#pragma once

#include <math.h>
#include "Common/BaneMacros.h"

#include "4ComponentVector.h"
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

template<typename T, class TDeterminer>
inline T lerp(const T& Lhs, const T& Rhs, TDeterminer Factor)
{
	float m1Factor = 1 - Factor;
	return T((Lhs * Factor) + (Rhs * m1Factor));
}

inline float length(const fvec4& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const fvec3& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const fvec2& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline fvec4 normalized(const fvec4& InType)
{
	return InType / length(InType);
}

inline fvec3 normalized(const fvec3& InType)
{
	return InType / length(InType);
}

inline fvec2 normalized(const fvec2& InType)
{
	return InType / length(InType);
}

inline fvec4& normalize(fvec4& InType)
{
	return InType /= length(InType);
}

inline fvec3& normalize(fvec3& InType)
{
	return InType /= length(InType);
}

inline fvec2& normalize(fvec2& InType)
{
	return InType /= length(InType);
}

inline float dot(const fvec4& Lhs, const fvec4& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const fvec3& Lhs, const fvec3& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const fvec2& Lhs, const fvec2& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float angleBetween(const fvec2& Lhs, const fvec2& Rhs)
{
	return degrees(acosf(dot(Lhs, Rhs)));
}

inline float angleBetween(const fvec3& Lhs, const fvec3& Rhs)
{
	return degrees(acosf(dot(Lhs, Rhs)));
}

inline fvec4 reflect(const fvec4& Incidence, const fvec4& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline fvec3 reflect(const fvec3& Incidence, const fvec3& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline fvec2 reflect(const fvec2& Incidence, const fvec2& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline fvec4 refract(const fvec4& Incidence, const fvec4& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return fvec4();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

inline fvec3 refract(const fvec3& Incidence, const fvec3& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return fvec3();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

inline fvec2 refract(const fvec2& Incidence, const fvec2& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return fvec2();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}


inline double lengthSqrd(const vec4& InType)
{
	return (InType.p[0] * InType.p[0]) + (InType.p[1] * InType.p[1]) + (InType.p[2] * InType.p[2]) + (InType.p[3] * InType.p[3]);
}

inline double length(const vec4& InType)
{
	return sqrt(lengthSqrd(InType));
}

inline double lengthSqrd(const vec3& InType)
{
	return (InType.p[0] * InType.p[0]) + (InType.p[1] * InType.p[1]) + (InType.p[2] * InType.p[2]);
}

inline double length(const vec3& InType)
{
	return sqrt(lengthSqrd(InType));
}

inline double lengthSqrd(const vec2& InType)
{
	return (InType.p[0] * InType.p[0]) + (InType.p[1] * InType.p[1]);
}

inline double length(const vec2& InType)
{
	return sqrt(lengthSqrd(InType));
}

inline vec4 normalized(const vec4& InType)
{
	return InType / length(InType);
}

inline vec3 normalized(const vec3& InType)
{
	return InType / length(InType);
}

inline vec2 normalized(const vec2& InType)
{
	return InType / length(InType);
}

inline vec4& normalize(vec4& InType)
{
	return InType /= length(InType);
}

inline vec3& normalize(vec3& InType)
{
	return InType /= length(InType);
}

inline vec2& normalize(vec2& InType)
{
	return InType /= length(InType);
}

inline double dot(const vec4& Lhs, const vec4& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline double dot(const vec3& Lhs, const vec3& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline double dot(const vec2& Lhs, const vec2& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline vec4 reflect(const vec4& Incidence, const vec4& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline vec3 reflect(const vec3& Incidence, const vec3& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline vec2 reflect(const vec2& Incidence, const vec2& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline vec4 refract(const vec4& Incidence, const vec4& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return vec4();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}

inline vec3 refract(const vec3& Incidence, const vec3& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return vec3();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}

inline vec2 refract(const vec2& Incidence, const vec2& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return vec2();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}

inline double angleBetween(const vec3& Lhs, const vec3& Rhs)
{
	return degreesD(acos(dot(Lhs, Rhs)));
}

inline double angleBetween(const vec2& Lhs, const vec2& Rhs)
{
	return degreesD(acos(dot(Lhs, Rhs)));
}

inline fvec3 cross(const fvec3& Lhs, const fvec3& Rhs)
{
	return fvec3(
		Lhs.y * Rhs.z - Rhs.y * Lhs.z,
		Lhs.z * Rhs.x - Rhs.z * Lhs.x,
		Lhs.x * Rhs.y - Rhs.x * Lhs.y
	);
}

inline vec3 cross(const vec3& Lhs, const vec3& Rhs)
{
	return vec3(
		Lhs.y * Rhs.z - Rhs.y * Lhs.z,
		Lhs.z * Rhs.x - Rhs.z * Lhs.x,
		Lhs.x * Rhs.y - Rhs.x * Lhs.y
	);
}

template<typename T>
inline T saturate(const T& Value)
{
	for (uint32 i = 0; i < T::ColCount; i++)
	{
		if (Value[i] < 0.0f) Value[i] = 0.0f;
		else if (Value[i] > 1.f) Value[i] = 1.0f;
	}
	return Value;
}

template<>
inline float saturate(const float& Value)
{
	if (Value < 0.0f) return 0.0f;
	else if (Value > 1.0f) return 1.0f;
	return Value;
}

template<>
inline double saturate(const double& Value)
{
	if (Value < 0.0) return 0.0;
	else if (Value > 1.0) return 1.0;
	return Value;
}

inline matrix matProjection(float Aspect, float FovY, float Near, float Far)
{
	FovY = radians(FovY);
	float TanFovOverTwo = tanf(FovY / 2.f);
	matrix Result(
		(1.f / (Aspect * TanFovOverTwo)), 0.f, 0.f, 0.f,
		0.f, -1.f / TanFovOverTwo, 0.f, 0.f,
		0.f, 0.f, 1.0f, -((Far * Near) / (Far - Near)),
		0.f, 0.f, 1.f, 0.f
	);
	return Result;
}

template<class T>
inline bool isNan(const T& InVal)
{
	T::T Test = T::T(0);
	for (uint32 i = 0; i < T::ColCount; i++)
	{
		Test += InVal[i];
	}
	return isnan(Test);
}

template<class T>
inline T Abs(const T& InVal)
{
	T Result;
	for (uint32 i = 0; i < T::ColCount; i++)
	{
		Result[i] = abs(Result[i]);
	}
	return Result;
}

// inline matrix xRotation(float Degrees)
// {
// 
// }
// 
// inline matrix yRotation(float Degrees)
// {
// 
// }
// 
// inline matrix zRotation(float Degrees)
// {
// 
// }

// TODO: implement all the variations of this function
// T transpose(const T& InMatrix)
// {
// 	T Result;
// 	for (uint32 y = 0; y < T::NumRows; y++)
// 	{
// 		for (uint32 x = 0; x < T::NumCols; x++)
// 		{
// 			Result[y][x] = InMatrix[x][y];
// 		}
// 	}
// 	return Result;
// }


// Forward is expected to be normalized already
inline matrix matView(fvec3 Eye, fvec3 Forward, fvec3 Up)
{
	BANE_CHECK(!(Forward.x == 0.0 && Forward.y == 0.0 && Forward.z == 0.0));

	const fvec3 ZAxis = normalized(Forward);
	const fvec3 XAxis = normalized(cross(Up, ZAxis));
	const fvec3 YAxis = cross(XAxis, ZAxis);

	matrix Result(
		fvec4(XAxis.x, XAxis.y, XAxis.z, 0.0f),
		fvec4(YAxis.x, YAxis.y, YAxis.z, 0.0f),
		fvec4(ZAxis.x, ZAxis.y, ZAxis.z, 0.0f),
		fvec4(0.0f, 0.0f, 0.0f, 1.0f)
	);

// 	matrix Result(
// 		fvec4(XAxis.x, YAxis.x, ZAxis.x, 0.0f),
// 		fvec4(XAxis.y, YAxis.y, ZAxis.y, 0.0f),
// 		fvec4(XAxis.z, YAxis.z, ZAxis.z, 0.0f),
// 		fvec4(-dot(XAxis, Eye), -dot(YAxis, Eye), -dot(ZAxis, Eye), 1.0f)
// 	);

	return Result;
}

inline matrix matTranslation(const fvec3& Position)
{
	matrix Result(1.0f);
	Result.p[0][3] = Position.x;
	Result.p[1][3] = Position.y;
	Result.p[2][3] = Position.z;
	return Result;
}


inline matrix matScale(const fvec3& Scale)
{
	matrix Result(1.0f);

	Result.p[0][0] = Scale.x;
	Result.p[1][1] = Scale.y;
	Result.p[2][2] = Scale.z;

	return Result;
}

inline matrix matRotX(float Radians)
{
	return matrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, cosf(Radians), -sinf(Radians), 0,
		0.f, sinf(Radians), cosf(Radians), 0,
		0.f, 0.f, 0.f, 1.f
	);
}

inline matrix matRotY(float Radians)
{
	return matrix(
		cosf(Radians), 0.f, sinf(Radians), 0.f,
		0.f, 1.f, 0.f, 0.f,
		-sinf(Radians), 0.f, cosf(Radians), 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

inline matrix matRotZ(float Radians)
{
	return matrix(
		cosf(Radians), -sinf(Radians), 0.f, 0.f,
		sinf(Radians), cosf(Radians), 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

enum NoName
{
	Name
};

inline mat4x4 matProjection(float InAspect, float InFovY, float InNear, float InFar, NoName N)
{
	while (0) { (void)N; }
	double Aspect = static_cast<double>(InAspect);
	double FovY = static_cast<double>(InFovY);
	double Near = static_cast<double>(InNear);
	double Far = static_cast<double>(InFar);
	FovY = radiansD(FovY);
	double TanFovOverTwo = tan(FovY / 2.);
	mat4x4 Result(
		-1. / (Aspect * TanFovOverTwo), 0., 0., 0.,
		0., 1. / TanFovOverTwo, 0., 0.,
		0., 0., -((Far + Near) / (Far - Near)), -((2 * Far * Near) / (Far - Near)),
		0., 0., -1., 0.
	);
	return Result;
}

inline mat4x4 matView(vec3 Eye, vec3 Forward, vec3 Up)
{
	const vec3 ZAxis = normalized(Eye - Forward);
	const vec3 XAxis = normalized(cross(Up, ZAxis));
	const vec3 YAxis = cross(ZAxis, XAxis);

	mat4x4 Result(
		vec4(XAxis.x, XAxis.y, XAxis.z, -dot(XAxis, Eye)),
		vec4(YAxis.x, YAxis.y, YAxis.z, -dot(YAxis, Eye)),
		vec4(ZAxis.x, ZAxis.y, ZAxis.z, -dot(ZAxis, Eye)),
		vec4(0.0, 0.0, 0.0, 1.0)
	);

	return Result;
}

inline mat4x4 matTranslation(const vec3& Position)
{
	mat4x4 Result(1.0);
	Result[0][3] = Position.x;
	Result[1][3] = Position.y;
	Result[2][3] = Position.z;
	return Result;
}


inline mat4x4 matScale(const vec3& Scale)
{
	mat4x4 Result(1.0);

	Result[0][0] = Scale.x;
	Result[1][1] = Scale.y;
	Result[2][2] = Scale.z;

	return Result;
}

inline vec3 RandomVec3(double Magnitude = -1.0)
{
	std::random_device Dev;
	std::mt19937 Gen(Dev());
	std::uniform_real_distribution<double> Dist;
	vec3 Result = vec3(Dist(Gen), Dist(Gen), Dist(Gen));
	if (Magnitude > 0.0)
	{
		normalize(Result) * Magnitude;
	}
	return Result;
}