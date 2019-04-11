#pragma once

#include "PhysicsData.h"

class Planet
{
public:

	PhysicsProperties Props;

	CurrentPhysicsData CurrentPhysics;
	NextPhysicsData NextPhysics;
	FuturePhysicsData FuturePhysics;

};