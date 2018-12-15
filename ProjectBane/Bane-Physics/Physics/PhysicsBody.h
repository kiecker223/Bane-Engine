#pragma once
#include <KieckerMath.h>
#include <vector>
#include "PhysicsMesh.h"
#include "PhysicsCylinderShape.h"


typedef enum EPHYSICS_BODY_TYPE {
	PHYSICS_BODY_TYPE_SPHERE,
	PHYSICS_BODY_TYPE_CYLINDER,
	PHYSICS_BODY_TYPE_MESH
} EPHYSICS_BODY_TYPE;

typedef struct PHYSICS_SPHERE_INFO {
	double Radius;
} PHYSICS_SPHERE_INFO;

typedef struct PHYSICS_CYLINDER_INFO {
	double Height;
	double Radius;
} PHYSICS_CYLINDER_INFO;

typedef struct PHYSICS_BODY_CREATE_INFO {
	Quaternion Orientation;
	double3 Position;
	double3 Velocity;
	double3 AngularVelocity;
	double Mass;
	EPHYSICS_BODY_TYPE BodyType;
	PhysicsMesh PhysMesh;
	PHYSICS_SPHERE_INFO Sphere;
	PHYSICS_CYLINDER_INFO Cylinder;
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
		if (BodyType == PHYSICS_BODY_TYPE_CYLINDER)
		{
			Cylinder = Other.Cylinder;
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
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE)
		{
			Sphere = CreateInfo.Sphere;
		}
		else if (BodyType == PHYSICS_BODY_TYPE_CYLINDER)
		{
			Cylinder.Height = CreateInfo.Cylinder.Height;
			Cylinder.Radius = CreateInfo.Cylinder.Radius;
		}
		else if (BodyType == PHYSICS_BODY_TYPE_MESH)
		{
			PhysMesh = CreateInfo.PhysMesh;
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
	PhysicsCylinderShape Cylinder;

	inline double HitSphere(const double3& RayPos, const double3& RayDir, double Radius)
	{
		double3 OriginMCenter = RayPos - Position;
		double A = dot(RayDir, RayDir);
		double B = 2 * dot(OriginMCenter, RayDir);
		double C = dot(OriginMCenter, OriginMCenter) - (Radius * Radius);
		double Discriminate = (B * B) - (4 * A * C);
		double SqrDiscriminate = sqrt(Discriminate);
		if (isnan(SqrDiscriminate))
		{
			return -1.;
		}
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

// 	inline bool TestIntersection(const PhysicsBody& Rhs)
// 	{
// 
// 	}

	inline double TestRayHit(const double3& RayStart, const double3& RayDir, double3& OutNormal)
	{
		double Result = -1.;
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE) 
		{
			Result = HitSphere(RayStart, RayDir, Sphere.Radius);
			OutNormal = (RayStart + (RayDir * Result)) - Position;
		}
		else if (BodyType == PHYSICS_BODY_TYPE_CYLINDER)
		{
			Result = Cylinder.TestRayHit(RayStart, RayDir, Orientation, OutNormal);
		}
		else if (BodyType == PHYSICS_BODY_TYPE_MESH)
		{
			for (uint32 i = 0; i < PhysMesh.Faces.size(); i++)
			{
				PhysicsMesh::Face OrientedFace = PhysMesh.GetFaceOriented(i, Position, Orientation);
				double TestedHit = OrientedFace.TestRayHit(RayStart, RayDir, OutNormal);
				if (TestedHit > -1.)
				{
					return TestedHit;
				}
			}
		}
		return Result;
	}

	inline BoundingBox GetBounds() const 
	{
		BoundingBox Result;
		if (BodyType == PHYSICS_BODY_TYPE_SPHERE)
		{
			Result.Min = double3(-Sphere.Radius, -Sphere.Radius, -Sphere.Radius) + Position;
			Result.Max = double3(Sphere.Radius, Sphere.Radius, Sphere.Radius) + Position;
		}
		else if (BodyType == PHYSICS_BODY_TYPE_CYLINDER)
		{
			// H = half, C = cylinder, H = height
			double HCH = Cylinder.Height / 2.;
			Result.Min = Position - double3(HCH, HCH, HCH);
			Result.Max = Position + double3(HCH, HCH, HCH);
		}
		else
		{
			Result = PhysMesh.Bounds;
			Result.Min += Position;
			Result.Max += Position;
		}
		return Result;
	}

	uint64 EntityHandle;
	uint32 Handle;
};
