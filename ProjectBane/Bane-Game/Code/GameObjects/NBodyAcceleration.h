#pragma once

#include "PhysicsData.h"
#include "TaskSystem/MainTaskSystem.h"

class NBodyAcceleration
{
public:

	// This does return a dispatched Task handle
	Task* InitiateConstruct(std::vector<PhysicsData>& InPhysicsData);

	// This returns a dispatched Task handle
	Task* InitiateSimulate();

private:
};
