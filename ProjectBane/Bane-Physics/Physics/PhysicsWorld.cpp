#include "PhysicsWorld.h"
#include <iostream>

// Gravitational constant  G = (6.6747*10^-11)
// Gravitational equation F = G*((m1*m2)/r*r)


void PhysicsWorld::SpawnThread()
{
	PhysicsThread = std::move(std::thread([this]()
	{
		this->UpdatePhysics();
	}));
}

bool PhysicsWorld::CastRay(const PHYSICS_RAY& InRay, PhysicsBody& HitBody)
{
	UNUSED(InRay);
	UNUSED(HitBody);
	return false;
}

void PhysicsWorld::CastRayAtSpeedOfLight(const PHYSICS_RAY& InRay, std::function<PhysicsBody()>& HitFunc)
{
	UNUSED(InRay); UNUSED(HitFunc);
}

void PhysicsWorld::UpdatePhysics()
{
	// At the start lock the physics buffer
	while (bRunningPhysicsSim)
	{
		using Clock = std::chrono::high_resolution_clock;
		auto Start = Clock::now();
		if (AddList.GetElementCount() > 0)
		{
			std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
			for (auto& Body : AddList)
			{
				m_Bodies.Add(Body);
			}
			AddList.Empty();
		}

		if (MessageQueue.GetMessageCount() > 0)
		{
			auto* Message = MessageQueue.GetMessage(0);
			while (Message->pNext)
			{
				if (Message->bQuit)
				{
					bRunningPhysicsSim = false;
				}
				else
				{
					Message->Execute(m_Bodies[Message->BodyId]);
					Message = Message->pNext;
				}
			}
		}
		if (true) continue;
		RegenerateOctTree();
		m_bUnlockedForRead = false;
		for (auto& Body : m_Bodies)
		{
			for (auto& OtherBody : m_Bodies)
			{
				if (OtherBody.Handle == Body.Handle)
				{
					continue;
				}
				double3 ForceDir = OtherBody.Position - Body.Position;
				double DistanceFromBody = length(ForceDir);
				normalize(ForceDir);
				
				if (DistanceFromBody < 1e-1) 
				{
					continue;
				}

				double Force = M_GRAV_CONST * ((Body.Mass * OtherBody.Mass) / (DistanceFromBody * DistanceFromBody));
				ForceDir *= Force;
				double3 AccelerationDir = ForceDir / Body.Mass;
				Body.Velocity += (AccelerationDir * (1. / 60.));
				if (!isNan(Body.Velocity))
				{
					Body.Position += Body.Velocity;
					if (isNan(Body.Position))
					{
						__debugbreak();
					}
				}
				else
				{
					__debugbreak();
				}
			}
		}
		UpdateBuffer.Bodies = m_Bodies;
		m_bUnlockedForRead = true;
		auto TimeTaken = Clock::now() - Start;
		std::chrono::nanoseconds SleepTime = (std::chrono::nanoseconds(16666667) - std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken));
		std::this_thread::sleep_for(SleepTime);
	}
	std::cout << "Quiting physics thread" << std::endl;
}

void PhysicsWorld::RegenerateOctTree()
{
	std::scoped_lock<std::mutex> ScopeGuard(GenerateOctTreeMutex);
	if (m_OctTree.Tail)
	{
		m_OctTree.RecursiveDelete();
		m_OctTree.Initialize();
	}
	// Thread safety!
	if (!m_bUnlockedForRead)
	{
		return;
	}

	TArray<uint32> AllIds;
	for (auto& Body : m_Bodies)
	{
		AllIds.Add(Body.Handle);
	}

	BoundingBox EntireScene = CalculateBoundsForMeshes(AllIds);
	
	auto* RootNode = m_OctTree.Tail;
	RootNode->Value.Bounds = EntireScene;
	RootNode->Value.MeshesInBounds = AllIds;
	GenerateOctTreeImpl(RootNode);
}

void PhysicsWorld::GenerateOctTreeImpl(TBinaryTree<OctTreeNode>::TNode* InNode)
{
	if (InNode->Value.MeshesInBounds.GetElementCount() > 1)
	{
		uint32 BiggestAxis = InNode->Value.Bounds.GetBiggestAxis();
		double3 Center = InNode->Value.Bounds.GetCenter();
		TArray<uint32> LNodeMeshes;
		TArray<uint32> RNodeMeshes;
		for (auto& Id : InNode->Value.MeshesInBounds)
		{
			auto& Body = GetBody(Id);
			if (Body.Position[BiggestAxis] > Center[BiggestAxis])
			{
				LNodeMeshes.Add(Id);
			}
			else
			{
				RNodeMeshes.Add(Id);
			}
		}
		BoundingBox LNodeBounds = CalculateBoundsForMeshes(LNodeMeshes);
		BoundingBox RNodeBounds = CalculateBoundsForMeshes(RNodeMeshes);
		auto* LNode = m_OctTree.CreateNode(InNode, { LNodeBounds, LNodeMeshes });
		auto* RNode = m_OctTree.CreateNode(InNode, { RNodeBounds, RNodeMeshes });
		InNode->NextL = LNode;
		InNode->NextR = RNode;
		GenerateOctTreeImpl(LNode);
		GenerateOctTreeImpl(RNode);
	}
}

BoundingBox PhysicsWorld::CalculateBoundsForMeshes(const TArray<uint32>& MeshHandles)
{
	BoundingBox Result;
	TArray<BoundingBox> Bounds;
	for (auto& Handle : MeshHandles)
	{
		Bounds.Add(m_Bodies[Handle].GetBounds());
	}
	Result.CalculateFromArray(reinterpret_cast<double3*>(Bounds.GetData()), Bounds.GetElementCount() * 2);
	return Result;
}
