#pragma once
#ifndef __BANE_MATH_H__
#define __BANE_MATH_H__

#include "Common/Types.h"
#include <math.h>
#include <string>


// Disable anonymous structures warnings
#pragma warning(disable:4201)

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


#include "Math/MathFunctions.h"

template<>
inline Quaternion lerp<Quaternion>(const Quaternion& V0, const Quaternion& V1, float Factor)
{
	float f = 1 - Factor;
	return Quaternion(V0.w * f + V1.w * Factor, V0.x * f + V1.x * Factor, V0.y * f + V1.y * Factor, V0.z * f + V1.z * Factor).Normalized();
}

#endif //__BANE_MATH_H__
