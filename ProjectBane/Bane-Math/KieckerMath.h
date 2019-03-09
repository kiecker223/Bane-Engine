#pragma once
#ifndef __BANE_MATH_H__
#define __BANE_MATH_H__

#include "Common/Types.h"
#include <math.h>
#include <string>


// Disable anonymous structures warnings
#pragma warning(disable:4201)



#include "Math/Quaternion.h"

template<>
inline Quaternion lerp<Quaternion, float>(const Quaternion& V0, const Quaternion& V1, float Factor)
{
	float f = 1 - Factor;
	return Quaternion(V0.w * f + V1.w * Factor, V0.x * f + V1.x * Factor, V0.y * f + V1.y * Factor, V0.z * f + V1.z * Factor).Normalized();
}

template<>
inline Quaternion64 lerp<Quaternion64, double>(const Quaternion64& V0, const Quaternion64& V1, double Factor)
{
	double f = 1 - Factor;
	return Quaternion64(V0.w * f + V1.w * Factor, V0.x * f + V1.x * Factor, V0.y * f + V1.y * Factor, V0.z * f + V1.z * Factor).Normalized();
}

#endif //__BANE_MATH_H__
