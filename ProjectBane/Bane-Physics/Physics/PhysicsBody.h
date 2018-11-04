#pragma once
#include <KieckerMath.h>
#include <Core/Containers/Array.h>
#include "PhysicsMesh.h"


typedef enum EPHYSICS_BODY_TYPE {
	PHYSICS_BODY_TYPE_SPHERE,
	PHYSICS_BODY_TYPE_MESH
} EPHYSICS_BODY_TYPE;

typedef struct PHYSICS_SPHERE_INFO {
	double Radius;
} PHYSICS_SPHERE_INFO;

typedef struct PHYSICS_BODY_CREATE_INFO {
	Quaternion Orientation;
	double3 Position;
	double3 Velocity;
	double3 AngularVelocity;
	double Mass;
	EPHYSICS_BODY_TYPE BodyType;
	PhysicsMesh PhysMesh;
	PHYSICS_SPHERE_INFO Sphere;
	uint32 Handle;
	uint64 EntityHandle;
} PHYSICS_BODY_CREATE_INFO;

class PhysicsBody
{
public:

	PhysicsBody() { }
	PhysicsBody(const PhysicsBody& Other) :
		Position(Other.Position),
		Velocity(Other.Velocity),
		Orientation(Other.Orientation),
		AngularVelocity(Other.AngularVelocity),
		Mass(Other.Mass),
		BodyType(Other.BodyType),
		EntityHandle(Other.EntityHandle),
		Handle(Other.Handle)
	{ 
		if (BodyType == PHYSICS_BODY_TYPE_MESH)
		{
			PhysMesh = Other.PhysMesh;
		}
		else
		{
			Sphere = Other.Sphere;
		}
	}
	PhysicsBody(const PHYSICS_BODY_CREATE_INFO& CreateInfo) :
		Position(CreateInfo.Position),
		Velocity(CreateInfo.Velocity),
		Orientation(CreateInfo.Orientation),
		AngularVelocity(CreateInfo.AngularVelocity),
		Mass(CreateInfo.Mass),
		BodyType(CreateInfo.BodyType),
		EntityHandle(CreateInfo.EntityHandle),
		Handle(CreateInfo.Handle)
	{
		if (BodyType == PHYSICS_BODY_TYPE_MESH)
		{
			PhysMesh = CreateInfo.PhysMesh;
		}
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE)
		{
			Sphere = CreateInfo.Sphere;
		}
	}
	~PhysicsBody() { }

	double3 Position;
	double3 Velocity;
	Quaternion Orientation;
	double3 AngularVelocity;
	double  Mass;

	void Accelerate(double3 Direction, double Magnitude);
	
	EPHYSICS_BODY_TYPE BodyType;
	PhysicsMesh PhysMesh;
	PHYSICS_SPHERE_INFO Sphere;

	uint64 EntityHandle;
	uint32 Handle;
};
