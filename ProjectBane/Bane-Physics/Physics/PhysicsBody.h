#pragma once
#include <KieckerMath.h>
#include <vector>


typedef enum EPHYSICS_BODY_TYPE {
	PHYSICS_BODY_TYPE_SPHERE,
	PHYSICS_BODY_TYPE_MESH
} EPHYSICS_BODY_TYPE;

// Currently assumes that everythings a sphere
class PhysicsBody
{
public:
	double  Radius;
	double3 Velocity;
	double  Mass;
	double3 AngularVelocity;

// 	void Accelerate(double3 Direction, double Magnitude) { }

	uint32  Handle;
	double3 Position;
};
