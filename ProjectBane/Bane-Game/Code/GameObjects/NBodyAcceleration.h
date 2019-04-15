#pragma once

#include "PhysicsData.h"
#include "TaskSystem/MainTaskSystem.h"


class NBodyAcceleration
{
public:

	void Construct(std::vector<PhysicsData>& InOutPhysicsData);

	void Simulate();

	// This does return a dispatched Task handle
	Task* InitiateConstruct(std::vector<PhysicsData>& InPhysicsData);

	// This returns a dispatched Task handle
	Task* InitiateSimulate();

private:
	
	class TreeBuildData
	{
	public:



	};

	class TreeBuilder
	{
	public:

		void Initialize(uint32 ThreadCount);

		void IndividualThreadFunc(uint32 ThreadIdx);


		struct WorkList
		{
		public:
			
		};

	};

	

};
