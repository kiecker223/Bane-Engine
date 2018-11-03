#pragma once
#include <KieckerMath.h>
#include <vector>
#include "PhysicsMesh.h"


typedef enum EPHYSICS_BODY_TYPE {
	PHYSICS_BODY_TYPE_SPHERE,
	PHYSICS_BODY_TYPE_MESH
} EPHYSICS_BODY_TYPE;

typedef struct PHYSICS_SPHERE_INFO {
	double Radius;
};

class PhysicsBody
{
public:
	double  Radius;
	double3 Velocity;
	double  Mass;
	double3 AngularVelocity;

	void Accelerate(double3 Direction, double Magnitude);
	
	EPHYSICS_BODY_TYPE BodyType;
	union {
		PhysicsMesh PhysMesh;
		PHYSICS_SPHERE_INFO Sphere;
	};

	uint64 EntityHandle;
	uint32  Handle;
	Quaternion Orientation;
	double3 Position;
};
