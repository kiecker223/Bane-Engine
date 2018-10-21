#pragma once
#include <KieckerMath.h>
#include <vector>


// Currently assumes that everythings a sphere
class PhysicsBody
{
public:
	double  Radius;
	double3 Velocity;
	double  Mass;

	uint32  Handle;
	double3 Position;
};
