#pragma once

#include <Common/Types.h>
#include <KieckerMath.h>

class PhysicsProperties
{
public:

	PhysicsProperties() : bCanTick(true), PhysicsWorldHandle(0)
	{
	}

	bool bCanTick;

	double Mass;
	double3 Velocity;
	uint32 PhysicsWorldHandle;
};
