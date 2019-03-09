#pragma once

#include "../Math/Quaternion.h"
#include "../Math/4ComponentVector.h"
#include <vector>

typedef enum ESHAPE_TYPE {
	SHAPE_TYPE_CYLINDER,
	SHAPE_TYPE_SPHERE,
	SHAPE_TYPE_PLANE,
	SHAPE_TYPE_TRIANGLELIST,
	SHAPE_TYPE_CUBE
} ESHAPE_TYPE;

struct Cylinder
{
	vec3 Position;
	vec3 Orientation;
	double Radius;
};

struct Sphere
{
	vec3 Position;
	double Radius;
};

struct Plane
{
	vec3 Origin;
	vec3 Normal;
};

struct Triangle
{
	vec3 Points[3];
	vec3 Normal; // Back or front facing
};

struct TriangleList
{
	std::vector<Triangle> Triangles;
};

struct Cube
{
	vec3 Center;
	Quaternion Orientation;
	vec3 Scale;
};

struct CollisionShape
{
	CollisionShape()
	{
		memset(this, 0, sizeof(this));
	}
	~CollisionShape()
	{

	}

	ESHAPE_TYPE ShapeType;
	union 
	{
		Cylinder CylinderData;
		Sphere SphereData;
		Plane PlaneData;
		TriangleList MeshData;
		Cube CubeData;
	};
};

struct Line
{
	vec3 PointA;
	vec3 PointB;
};

struct SweepingVolumes
{
	Cube LhsSweepingVolume;
	Cube RhsSweepingVolume;
};

SweepingVolumes ConstructSweepingVolumes(const CollisionShape& Lhs, const vec3& LhsVelocity, const CollisionShape& Rhs, const vec3& RhsVelocity);

struct CollisionData
{
	std::vector<vec3> ImpactNormals;
	std::vector<vec3> ImpactPositions;
	double TimeToCollision;
};

vec3 ClosestPointOnPlane(const vec3& InPoint, const Plane& InPlane);

vec3 ClosestPointToLineSegment(const Line& InLine, const vec3& InPoint, double& OutT);

double DistanceToLineSegment(const Line& InLine, const vec3& InPoint);

bool CheckCylinderCylinderCollision(const Cylinder& Lhs, const Cylinder& Rhs, CollisionData& OutCollision);
bool CheckCylinderSphereCollision(const Cylinder& Lhs, const Sphere& Rhs, CollisionData& OutCollision);
bool CheckCylinderPlaneCollision(const Cylinder& Lhs, const Plane& Rhs, CollisionData& OutCollision);
bool CheckCylinderTriangleListCollision(const Cylinder& Lhs, const TriangleList& Rhs, CollisionData& OutCollision);
bool CheckCylinderCubeCollision(const Cylinder& Lhs, const Cube& Rhs, CollisionData& OutCollision);

bool CheckSphereCylinderCollision(const Sphere& Lhs, const Cylinder& Rhs, CollisionData& OutCollision);
bool CheckSphereSphereCollision(const Sphere& Lhs, const Sphere& Rhs, CollisionData& OutCollision);
bool CheckSpherePlaneCollision(const Sphere& Lhs, const Plane& Rhs, CollisionData& OutCollision);
bool CheckSphereTriangleListCollision(const Sphere& Lhs, const TriangleList& Rhs, CollisionData& OutCollision);
bool CheckSphereCubeCollision(const Sphere& Lhs, const Cube& Rhs, CollisionData& OutCollision);

bool TestPlaneSphereNegativeSpace(const Plane& Lhs, const Sphere& Rhs);

bool CheckPlaneCylinderCollision(const Plane& Lhs, const Cylinder& Rhs, CollisionData& OutCollision);
bool CheckPlaneSphereCollision(const Plane& Lhs, const Sphere& Rhs, CollisionData& OutCollision);
bool CheckPlanePlaneCollision(const Plane& Lhs, const Plane& Rhs, CollisionData& OutCollision);
bool CheckPlaneTriangleListCollision(const Plane& Lhs, const TriangleList& Rhs, CollisionData& OutCollision);
bool CheckPlaneCubeCollision(const Plane& Lhs, const Cube& Rhs, CollisionData& OutCollision);

bool CheckTriangleListCylinderCollision(const TriangleList& Lhs, const Cylinder& Rhs, CollisionData& OutCollision);
bool CheckTriangleListSphereCollision(const TriangleList& Lhs, const Sphere& Rhs, CollisionData& OutCollision);
bool CheckTriangleListPlaneCollision(const TriangleList& Lhs, const Plane& Rhs, CollisionData& OutCollision);
bool CheckTriangleListTriangleListCollision(const TriangleList& Lhs, const TriangleList& Rhs, CollisionData& OutCollision);
bool CheckTriangleListCubeCollision(const TriangleList& Lhs, const Cube& Rhs, CollisionData& OutCollision);

bool CheckCubeCylinderCollision(const Cube& Lhs, const Cylinder& Rhs, CollisionData& OutCollision);
bool CheckCubeSphereCollision(const Cube& Lhs, const Sphere& Rhs, CollisionData& OutCollision);
bool CheckCubePlaneCollision(const Cube& Lhs, const Plane& Rhs, CollisionData& OutCollision);
bool CheckCubeTriangleListCollision(const Cube& Lhs, const TriangleList& Rhs, CollisionData& OutCollision);
bool CheckCubeCubeCollision(const Cube& Lhs, const Cube& Rhs, CollisionData& OutCollision);

bool CheckCollisionsSweeping(const CollisionShape& Lhs, const CollisionShape& Rhs, const SweepingVolumes& SweepingVols, CollisionData& OutData);
bool CheckCollisionOfShapes(const CollisionShape& Lhs, const CollisionShape& Rhs, CollisionData& OutData);

struct Ray
{
	vec3 Source;
	vec3 Direction;
	double TStart, TMax;
};


