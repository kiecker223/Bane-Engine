#pragma once

#include <KieckerMath.h>
#include <vector>
#include "CarrierShipPart.h"


class PhysicsProperties
{
public:

	void BuildCollisionShape(CarrierShipPart* BaseShipPart) { }
	void BuildCollisionShape(CombatShipPart* BaseShipPart) { }

	double Mass;

};

class CurrentPhysicsData
{
public:
	vec4 AngularVelocity;
	vec3 Position;
	vec3 Velocity;
};

class NextPhysicsData
{
public:
	vec4 AngularVelocity;
	vec3 Position;
	vec3 Velocity;
};

class FuturePhysicsData
{
public:
	std::vector<vec4> AngularVelocities;
	int32 DirtyAngularVelocityIndex;
	double DirtyAngularVelocityTime;

	std::vector<vec3> Positions;
	int32 DirtyPositionIndex;
	double DirtyPositionTime;

	std::vector<vec3> Velocities;
	int32 DirtyVelocityIndex;
	double DirtyVelocityTime;
};
