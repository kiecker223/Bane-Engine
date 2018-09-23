#pragma once
#include <KieckerMath.h>


// Currently assumes that everythings a sphere
class PhysicsBody
{
public:

	float Radius;
	float3 Velocity;
	float Mass;

	uint32 Handle;
	float3 Position;

};

