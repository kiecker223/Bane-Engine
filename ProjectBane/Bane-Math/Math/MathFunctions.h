#pragma once

#include <math.h>
#include "Common/BaneMacros.h"

#include "Quaternion.h"
#include "4ComponentVector.h"
#include "4RowColMatrix.h"


template<typename T, class TDeterminer>
inline T lerp(const T& Lhs, const T& Rhs, TDeterminer Factor)
{
	float m1Factor = 1 - Factor;
	return T((Lhs * Factor) + (Rhs * m1Factor));
}

inline float length(const float4& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const float3& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const float2& InType)
{
	float Result = 0.f;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float4 normalized(const float4& InType)
{
	return InType / length(InType);
}

inline float3 normalized(const float3& InType)
{
	return InType / length(InType);
}

inline float2 normalized(const float2& InType)
{
	return InType / length(InType);
}

inline float4& normalize(float4& InType)
{
	return InType /= length(InType);
}

inline float3& normalize(float3& InType)
{
	return InType /= length(InType);
}

inline float2& normalize(float2& InType)
{
	return InType /= length(InType);
}

inline float dot(const float4& Lhs, const float4& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const float3& Lhs, const float3& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const float2& Lhs, const float2& Rhs)
{
	float Result = 0;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float4 reflect(const float4& Incidence, const float4& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline float3 reflect(const float3& Incidence, const float3& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline float2 reflect(const float2& Incidence, const float2& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline float4 refract(const float4& Incidence, const float4& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return float4();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

inline float3 refract(const float3& Incidence, const float3& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return float3();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

inline float2 refract(const float2& Incidence, const float2& Normal, float IndicesToRefraction)
{
	float NDotI = dot(Normal, Incidence);
	float K = 1.0f - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0f)
	{
		return float2();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrtf(K)));
}

inline double length(const double4& InType)
{
	double Result = 0.;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrt(Result);
	return Result;
}

inline double length(const double3& InType)
{
	double Result = 0.;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrt(Result);
	return Result;
}

inline double length(const double2& InType)
{
	double Result = 0.;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrt(Result);
	return Result;
}

inline double4 normalized(const double4& InType)
{
	return InType / length(InType);
}

inline double3 normalized(const double3& InType)
{
	return InType / length(InType);
}

inline double2 normalized(const double2& InType)
{
	return InType / length(InType);
}

inline double4& normalize(double4& InType)
{
	return InType /= length(InType);
}

inline double3& normalize(double3& InType)
{
	return InType /= length(InType);
}

inline double2& normalize(double2& InType)
{
	return InType /= length(InType);
}

inline double dot(const double4& Lhs, const double4& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 4; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline double dot(const double3& Lhs, const double3& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 3; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline double dot(const double2& Lhs, const double2& Rhs)
{
	double Result = 0;
	for (uint32 i = 0; i < 2; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline double4 reflect(const double4& Incidence, const double4& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline double3 reflect(const double3& Incidence, const double3& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline double2 reflect(const double2& Incidence, const double2& Normal)
{
	return Incidence - (Normal * (2.0f * dot(Normal, Incidence)));
}

inline double4 refract(const double4& Incidence, const double4& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return double4();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}

inline double3 refract(const double3& Incidence, const double3& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return double3();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}

inline double2 refract(const double2& Incidence, const double2& Normal, double IndicesToRefraction)
{
	double NDotI = dot(Normal, Incidence);
	double K = 1.0 - IndicesToRefraction * IndicesToRefraction * (1 - NDotI * NDotI);
	if (K < 0.0)
	{
		return double2();
	}
	return (Incidence * IndicesToRefraction) - (Normal * (IndicesToRefraction * NDotI + sqrt(K)));
}


inline float3 cross(const float3& Lhs, const float3& Rhs)
{
	return float3(
		Lhs.y * Rhs.z - Rhs.y * Lhs.z,
		Lhs.z * Rhs.x - Rhs.z * Lhs.x,
		Lhs.x * Rhs.y - Rhs.x * Lhs.y
	);
}

inline double3 cross(const double3& Lhs, const double3& Rhs)
{
	return double3(
		Lhs.y * Rhs.z - Rhs.y * Lhs.z,
		Lhs.z * Rhs.x - Rhs.z * Lhs.x,
		Lhs.x * Rhs.y - Rhs.x * Lhs.y
	);
}

inline matrix matProjection(float Aspect, float FovY, float Near, float Far)
{
	FovY = radians(FovY);
	float TanFovOverTwo = tanf(FovY / 2.f);
	matrix Result(
		1.f / (Aspect * TanFovOverTwo), 0.f, 0.f, 0.f,
		0.f, 1.f / TanFovOverTwo, 0.f, 0.f,
		0.f, 0.f, -((Far + Near) / (Far - Near)), -((2 * Far * Near) / (Far - Near)),
		0.f, 0.f, -1.f, 0.f
	);
	return Result;
}

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

inline float4 fromDouble4(const double4& InVal)
{
	return float4(static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z), static_cast<float>(InVal.w));
}

inline float3 fromDouble3(const double3& InVal)
{
	return float3(static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z));
}

inline Quaternion FromQuat64(const Quaternion64& InVal)
{
	return Quaternion(static_cast<float>(InVal.w), static_cast<float>(InVal.x), static_cast<float>(InVal.y), static_cast<float>(InVal.z));
}

inline float2 fromDouble2(const double2& InVal)
{
	return float2(static_cast<float>(InVal.x), static_cast<float>(InVal.y));
}

inline double4 fromFloat4(const float4& InVal)
{
	return double4(static_cast<double>(InVal.x), static_cast<double>(InVal.y), static_cast<double>(InVal.z), static_cast<double>(InVal.w));
}

inline double3 fromFloat3(const float3& InVal)
{
	return double3(static_cast<double>(InVal.x), static_cast<double>(InVal.y), static_cast<double>(InVal.z));
}

inline double2 fromFloat2(const float2& InVal)
{
	return double2(static_cast<double>(InVal.x), static_cast<double>(InVal.y));
}

// Forward is expected to be normalized already
inline matrix matView(float3 Eye, float3 Forward, float3 Up)
{
	const float3 ZAxis = normalized(Eye - Forward);
	const float3 XAxis = normalized(cross(Up, ZAxis));
	const float3 YAxis = cross(ZAxis, XAxis);

	matrix Result(
		float4(XAxis.x, XAxis.y, XAxis.z, -dot(XAxis, Eye)),
		float4(YAxis.x, YAxis.y, YAxis.z, -dot(YAxis, Eye)),
		float4(ZAxis.x, ZAxis.y, ZAxis.z, -dot(ZAxis, Eye)),
		float4(0.0f, 0.0f, 0.0f, 1.0f)
	);

	return Result;
}

inline matrix matTranslation(const float3& Position)
{
	matrix Result(1.0f);
	Result[0][3] = Position.x;
	Result[1][3] = Position.y;
	Result[2][3] = Position.z;
	return Result;
}

inline matrix matRotation(const Quaternion& Rotation)
{
	return Rotation.RotationMatrix();
}

inline matrix matScale(const float3& Scale)
{
	matrix Result(1.0f);

	Result[0][0] = Scale.x;
	Result[1][1] = Scale.y;
	Result[2][2] = Scale.z;

	return Result;
}

inline matrix matTransformation(const float3& Position, const Quaternion& Rotation, const float3& Scale)
{
	matrix Result;

	const matrix m1 = matTranslation(Position);
	const matrix m2 = matRotation(Rotation);
	const matrix m3 = matScale(Scale);

	Result = m1 * m2 * m3;

	return Result;
}

enum NoName
{
	Name
};

inline double4x4 matProjection(float InAspect, float InFovY, float InNear, float InFar, NoName N)
{
	while (0) { (void)N; }
	double Aspect = static_cast<double>(InAspect);
	double FovY = static_cast<double>(InFovY);
	double Near = static_cast<double>(InNear);
	double Far = static_cast<double>(InFar);
	FovY = radiansD(FovY);
	double TanFovOverTwo = tan(FovY / 2.);
	double4x4 Result(
		1. / (Aspect * TanFovOverTwo), 0., 0., 0.,
		0., 1. / TanFovOverTwo, 0., 0.,
		0., 0., -((Far + Near) / (Far - Near)), -((2 * Far * Near) / (Far - Near)),
		0., 0., -1., 0.
	);
	return Result;
}

inline double4x4 matView(double3 Eye, double3 Forward, double3 Up)
{
	const double3 ZAxis = normalized(Eye - Forward);
	const double3 XAxis = normalized(cross(Up, ZAxis));
	const double3 YAxis = cross(ZAxis, XAxis);

	double4x4 Result(
		double4(XAxis.x, XAxis.y, XAxis.z, -dot(XAxis, Eye)),
		double4(YAxis.x, YAxis.y, YAxis.z, -dot(YAxis, Eye)),
		double4(ZAxis.x, ZAxis.y, ZAxis.z, -dot(ZAxis, Eye)),
		double4(0.0, 0.0, 0.0, 1.0)
	);

	return Result;
}

inline double4x4 matTranslation(const double3& Position)
{
	double4x4 Result(1.0);
	Result[0][3] = Position.x;
	Result[1][3] = Position.y;
	Result[2][3] = Position.z;
	return Result;
}

inline double4x4 matRotation(const Quaternion64& Rotation)
{
	return Rotation.RotationMatrix();
}

inline double4x4 matScale(const double3& Scale)
{
	double4x4 Result(1.0);

	Result[0][0] = Scale.x;
	Result[1][1] = Scale.y;
	Result[2][2] = Scale.z;

	return Result;
}

inline double4x4 matTransformation(const double3& Position, const Quaternion64& Rotation, const double3& Scale)
{
	double4x4 Result;

	const double4x4 m1 = matTranslation(Position);
	const double4x4 m2 = matRotation(Rotation);
	const double4x4 m3 = matScale(Scale);

	Result = m1 * m2 * m3;

	return Result;
}
