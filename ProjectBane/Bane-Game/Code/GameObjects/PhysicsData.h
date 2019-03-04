#pragma once

#include <KieckerMath.h>
#include <vector>

class PhysicsData
{
public:

	vec4 AngularVelocity; // (x, y, z) = Angle (w) = Power

	vec3 CurrentVelocity;
	vec3 NextVelocity;

	vec3 CurrentPosition;
	vec3 NextPosition;

	std::vector<vec3> AllNextVelocities;
	std::vector<vec3> AllNextPositions;

	int32 PositionDirtyIndex;
	double PositionDirtyTime;

	int32 VelocityDirtyIndex;
	double VelocityDirtyTime;
};