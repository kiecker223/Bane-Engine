#pragma once

#include <math.h>

#include "Quaternion.h"
#include "4ComponentVector.h"
#include "4RowColMatrix.h"


inline float length(const float4& InType)
{
	float Result = 0.f;
	for (uint i = 0; i < 4; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const float3& InType)
{
	float Result = 0.f;
	for (uint i = 0; i < 3; i++)
	{
		Result += (InType.p[i] * InType.p[i]);
	}
	Result = sqrtf(Result);
	return Result;
}

inline float length(const float2& InType)
{
	float Result = 0.f;
	for (uint i = 0; i < 2; i++)
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
	for (uint i = 0; i < 4; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const float3& Lhs, const float3& Rhs)
{
	float Result = 0;
	for (uint i = 0; i < 3; i++)
	{
		Result += (Lhs.p[i] * Rhs.p[i]);
	}
	return Result;
}

inline float dot(const float2& Lhs, const float2& Rhs)
{
	float Result = 0;
	for (uint i = 0; i < 2; i++)
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


template<class T>
inline T lerp(const T& V0, const T& V1, float Factor)
{
	return V0 * (1 - Factor) + V1 * Factor;
}

inline float3 cross(const float3& Lhs, const float3& Rhs)
{
	return float3(
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
// 	for (uint y = 0; y < T::NumRows; y++)
// 	{
// 		for (uint x = 0; x < T::NumCols; x++)
// 		{
// 			Result[y][x] = InMatrix[x][y];
// 		}
// 	}
// 	return Result;
// }



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
