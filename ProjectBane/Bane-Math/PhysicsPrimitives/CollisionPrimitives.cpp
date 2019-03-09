#include "CollisionPrimitives.h"
#include "../Math/MathFunctions.h"

SweepingVolumes ConstructSweepingVolumes(const CollisionShape& Lhs, const vec3& LhsVelocity, const CollisionShape& Rhs, const vec3& RhsVelocity)
{
	return {};
}

vec3 ClosestPointOnPlane(const vec3& InPoint, const Plane& InPlane)
{
	double T = dot(InPlane.Normal, InPoint);
	return InPoint - (InPlane.Normal * T);
}

vec3 ClosestPointToLineSegment(const Line& InLine, const vec3& InPoint, double& OutT)
{
	vec3 Ab = InLine.PointA - InLine.PointB;
	OutT = dot(InPoint - InLine.PointA, Ab) / dot(Ab, Ab);
	saturate(OutT);
	return InLine.PointA + (Ab * OutT);
}

double DistanceToLineSegment(const Line& InLine, const vec3& InPoint)
{
	vec3 Ab = InLine.PointB - InLine.PointA, Ac = InPoint - InLine.PointA, Bc = InPoint - InLine.PointB;
	double E = dot(Ac, Ab);
	if (E < 0.0f) return sqrt(dot(Ac, Ac));
	double F = dot(Ab, Ab);
	if (E >= F) return sqrt(dot(Bc, Bc));
	return (sqrt(dot(Ac, Ac) - (E * E / F)));
}

bool CheckCylinderCylinderCollision(const Cylinder& Lhs, const Cylinder& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCylinderSphereCollision(const Cylinder& Lhs, const Sphere& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCylinderPlaneCollision(const Cylinder& Lhs, const Plane& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCylinderTriangleListCollision(const Cylinder& Lhs, const TriangleList& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCylinderCubeCollision(const Cylinder& Lhs, const Cube& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckSphereCylinderCollision(const Sphere& Lhs, const Cylinder& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckSphereSphereCollision(const Sphere& Lhs, const Sphere& Rhs, CollisionData& OutCollision)
{
	vec3 D = Lhs.Position - Rhs.Position;
	double Dist2 = dot(D, D);
	double RadiusSum = Lhs.Radius + Rhs.Radius;
	return Dist2 <= RadiusSum * RadiusSum;
}

bool CheckSpherePlaneCollision(const Sphere& Lhs, const Plane& Rhs, CollisionData& OutCollision)
{
	double D = dot(Rhs.Normal, Rhs.Origin);
	double E = dot(Lhs.Position, Rhs.Normal) - D;
	
	return abs(E) < Lhs.Radius;
}

bool CheckSphereTriangleListCollision(const Sphere& Lhs, const TriangleList& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckSphereCubeCollision(const Sphere& Lhs, const Cube& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool TestPlaneSphereNegativeSpace(const Plane& Lhs, const Sphere& Rhs)
{
	double D = dot(Lhs.Normal, Lhs.Origin);
	double Dist = dot(Rhs.Position, Lhs.Normal) - D;
	return Dist <= Rhs.Radius;
}

bool CheckPlaneCylinderCollision(const Plane& Lhs, const Cylinder& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckPlaneSphereCollision(const Plane& Lhs, const Sphere& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckPlanePlaneCollision(const Plane& Lhs, const Plane& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckPlaneTriangleListCollision(const Plane& Lhs, const TriangleList& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckPlaneCubeCollision(const Plane& Lhs, const Cube& Rhs, CollisionData& OutCollision)
{
	double D = dot(Lhs.Normal, Lhs.Origin);
	vec3 XScale = fromFloat3(Rhs.Orientation * fvec3(1.0f, 0.0f, 0.0f)) / 2.0;
	vec3 YScale = fromFloat3(Rhs.Orientation * fvec3(0.0f, 1.0f, 0.0f)) / 2.0;
	vec3 ZScale = fromFloat3(Rhs.Orientation * fvec3(0.0f, 0.0f, 1.0f)) / 2.0;
	double Radius = (Rhs.Scale.x / 2.0) * (abs(dot(Lhs.Normal, XScale))) +
					(Rhs.Scale.y / 2.0) * (abs(dot(Lhs.Normal, YScale))) +
					(Rhs.Scale.z / 2.0) * (abs(dot(Lhs.Normal, ZScale)));
	double S = dot(Lhs.Normal, Rhs.Center) - D;
	return abs(S) <= Radius;
}

bool CheckTriangleListCylinderCollision(const TriangleList& Lhs, const Cylinder& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckTriangleListSphereCollision(const TriangleList& Lhs, const Sphere& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckTriangleListPlaneCollision(const TriangleList& Lhs, const Plane& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckTriangleListTriangleListCollision(const TriangleList& Lhs, const TriangleList& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckTriangleListCubeCollision(const TriangleList& Lhs, const Cube& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCubeCylinderCollision(const Cube& Lhs, const Cylinder& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCubeSphereCollision(const Cube& Lhs, const Sphere& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCubePlaneCollision(const Cube& Lhs, const Plane& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCubeTriangleListCollision(const Cube& Lhs, const TriangleList& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCubeCubeCollision(const Cube& Lhs, const Cube& Rhs, CollisionData& OutCollision)
{
	return false;
}

bool CheckCollisionsSweeping(const CollisionShape& Lhs, const CollisionShape& Rhs, const SweepingVolumes& SweepingVols, CollisionData& OutData)
{
	return false;
}

bool CheckCollisionOfShapes(const CollisionShape& Lhs, const CollisionShape& Rhs, CollisionData& OutData)
{
	return false;
}
