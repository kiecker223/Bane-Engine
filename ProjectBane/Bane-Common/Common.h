#pragma once

#include "Common/Types.h"
#include "Common/BaneMacros.h"
#include "Common/MemFunctions.h"

// Below function is needed in Core and in BaneMath, so I put it here since this project lies
// the lowest in the dependency tree


inline uint32 NextPowerOfTwo(uint32 Value)
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