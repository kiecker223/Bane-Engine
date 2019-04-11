#pragma once

#include <KieckerMath.h>
#include <vector>
#include "CarrierShipPart.h"


class PhysicsProperties
{
public:

	double Mass;

};

class PhysicsData
{
public:
	PhysicsProperties* Props;
	vec4 AngularVelocity;
	vec3 Position;
	vec3 Velocity;
};

typedef PhysicsData CurrentPhysicsData;
typedef PhysicsData NextPhysicsData;

class FuturePhysicsData
{
public:
	std::vector<PhysicsData> NewPhysicsData;
	uint32 DirtyPhysicsDataIndex;
	double DirtyPhysicsDataTime;
};
