#pragma once

#include "PhysicsData.h"

class Planet
{
public:

	std::string PlanetName;

	PhysicsProperties Props;

	CurrentPhysicsData CurrentPhysics;
	NextPhysicsData NextPhysics;
	FuturePhysicsData FuturePhysics;

};