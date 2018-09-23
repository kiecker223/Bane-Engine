#pragma once

#include <Common/Types.h>

class PhysicsProperties
{
public:

	PhysicsProperties() : bCanTick(true), PhysicsWorldHandle(0)
	{
	}

	bool bCanTick;

	float Mass;
	float3 Velocity;
	uint32 PhysicsWorldHandle;
};
