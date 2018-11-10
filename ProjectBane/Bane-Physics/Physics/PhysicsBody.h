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


	inline double HitSphere(const double3& RayPos, const double3& RayDir, double Radius)
	{
		double3 OriginMCenter = RayPos - Position;
		double A = dot(RayDir, RayDir);
		double B = 2 * dot(OriginMCenter, RayDir);
		double C = dot(OriginMCenter, OriginMCenter) - (Radius * Radius);
		double Discriminate = (B * B) - (4 * A * C);
		double SqrDiscriminate = sqrt(Discriminate);
		double T = (-B - SqrDiscriminate) / (2 * A);
		if (T < M_POSITIVE_INFINITY && T > 1e-3)
		{
			return T;
		}
		T = (-B + SqrDiscriminate) / (2 * A);
		if (T < M_POSITIVE_INFINITY && T > 1e-3)
		{
			return T;
		}
		return -1.;
	}

	inline double TestRayHit(const double3& RayStart, const double3& RayDir, double3& OutNormal)
	{
		double Result = -1.;
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE) 
		{
			Result = HitSphere(RayStart, RayDir, Sphere.Radius);
			OutNormal = (RayStart + (RayDir * Result)) - Position;
		}
		if (BodyType == PHYSICS_BODY_TYPE_MESH)
		{
			for (uint32 i = 0; i < PhysMesh.Faces.GetCount(); i++)
			{
			}
		}
		return Result;
	}

	inline BoundingBox GetBounds() const 
	{
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE)
		{
			BoundingBox Result;
			Result.Min = double3(-Sphere.Radius, -Sphere.Radius, -Sphere.Radius) + Position;
			Result.Max = double3(Sphere.Radius, Sphere.Radius, Sphere.Radius) + Position;
			return Result;
		}
		else
		{
			BoundingBox Result = PhysMesh.Bounds;
			Result.Min += Position;
			Result.Max += Position;
			return Result;
		}
	}

	uint64 EntityHandle;
	uint32 Handle;
};
