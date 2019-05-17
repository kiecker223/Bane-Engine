#pragma once

#include "PhysicsData.h"
#include "TaskSystem/MainTaskSystem.h"


class NBodyAcceleration
{
public:


	NBodyAcceleration() : Topmost(nullptr), m_ConstructTask(nullptr), m_SimulateTask(nullptr), m_FreeMemTask(nullptr) {}

	// Do a multi threaded construction of the barnes hut tree
	void Construct(std::vector<PhysicsData>& InOutPhysicsData);

	// Do a multi threaded simulation
	void Simulate(std::vector<PhysicsData>& InOutPhysicsData, double DeltaTime);

private:
	
	void SimulateImpl(std::vector<PhysicsData>& InOutPhysicsData, double DeltaTime, uint32 DispatchSize, uint32 DispatchIndex);

	class TreeBuilder
	{
	public:

		TreeBuilder() : ThreadCount(0), ThreadStack(nullptr), pOperatingData(nullptr) { }

		void Initialize(uint32 InThreadCount, NBodyNode* InTopmost, LinearAllocator& ParentAllocator, std::vector<PhysicsData>& InOutPhysicsData);

		void IndividualThreadFunc(uint32 ThreadIdx);

		void IndividualThreadFreeFunc(uint32 ThreadIdx);

		struct PerThreadData
		{
			PerThreadData() : Allocator(nullptr) { }
			LinearAllocator* Allocator;
			std::vector<NBodyNode*> AllocatedNodes;
		};
		ThreadSafeQueue<NBodyNode*> WorkQueue;
		uint32 ThreadCount;
		PerThreadData* ThreadStack;
		std::vector<PhysicsData>* pOperatingData;
		NBodyNode* Topmost;
	};


	Task* m_ConstructTask;
	Task* m_SimulateTask;
	Task* m_FreeMemTask;

	NBodyNode* Topmost;

	TreeBuilder m_TreeBuilder;
	LinearAllocator m_MemoryPool;

};
