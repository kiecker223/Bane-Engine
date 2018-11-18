#include "PhysicsWorld.h"
#include <iostream>



void PhysicsWorld::SpawnThread()
{
	PhysicsThread = std::move(std::thread([this]()
	{
		this->UpdatePhysics();
	}));
}

bool IntersectsBox(const BoundingBox& Box, const PHYSICS_RAY& InRay)
{
	for (uint32 i = 0; i < 3; i++)
	{
		double t0 = min((Box.Min[i] - InRay.Position[i]) / InRay.Direction[i], (Box.Max[i] - InRay.Position[i]) / InRay.Direction[i]);
		double t1 = max((Box.Min[i] - InRay.Position[i]) / InRay.Direction[i], (Box.Max[i] - InRay.Position[i]) / InRay.Direction[i]);

		double MinVal = max(t0, 1e-3);
		double MaxVal = min(t1, M_POSITIVE_INFINITY);
		if (MaxVal < MinVal)
		{
			return false;
		}
	}
	return true;
}

TBinaryTree<PhysicsWorld::OctTreeNode>::TNode* FindSmallestBoxIntersectedImpl(const PHYSICS_RAY& InRay, TBinaryTree<PhysicsWorld::OctTreeNode>::TNode* LastNode)
{
	if (LastNode->NextR && IntersectsBox(LastNode->NextR->Value.Bounds, InRay))
	{
		return FindSmallestBoxIntersectedImpl(InRay, LastNode->NextR);
	}
	else if (LastNode->NextL && IntersectsBox(LastNode->NextL->Value.Bounds, InRay))
	{
		return FindSmallestBoxIntersectedImpl(InRay, LastNode->NextL);
	}
	else
	{
		return LastNode;
	}
}

bool PhysicsWorld::CastRay(const PHYSICS_RAY& InRay, RAY_HIT_INFO& OutInfo)
{
	std::lock_guard<std::mutex> LockGuard(GenerateOctTreeMutex);

	auto* SmallestNode = FindSmallestBoxIntersectedImpl(InRay, m_OctTree.Tail);

	// We can assume that if it wasn't able to get to the smallest possible type of bounding box it just intersected
	// a parent box and missed it altogether
	if (SmallestNode->Value.MeshesInBounds.GetCount() == 1)
	{
		auto& Body = GetBody(SmallestNode->Value.MeshesInBounds[0]);
		double3 Normal;
		double Result = Body.TestRayHit(InRay.Position, InRay.Direction, Normal);
		if (Result > -1.)
		{
			OutInfo.Body = &Body;
			OutInfo.Position = InRay.Position + (InRay.Direction * Result);
			OutInfo.Normal = Normal;
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
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
		if (AddList.GetCount() > 0)
		{
			std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
			for (auto& Body : AddList)
			{
				m_Bodies.Add(Body);
			}
			AddList.Empty();
		}

		if (!MessageQueue.Queue.IsEmpty())
		{
			std::lock_guard<std::mutex> ScopedMutex(MessageQueue.PushMessageMutex);
			while (!MessageQueue.Queue.IsEmpty())
			{
				auto Val = MessageQueue.Queue.Dequeue();
				Val.Callback(GetBody(Val.Handle));
			}
		}

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
				if (Force > 0.0)
				{
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
				else
				{
					continue;
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
	uint32 CallDepth = 0;
	GenerateOctTreeImpl(RootNode, CallDepth);
}

void PhysicsWorld::GenerateOctTreeImpl(TBinaryTree<OctTreeNode>::TNode* InNode, uint32& CallDepth)
{
	if (CallDepth > 40)
	{
		__debugbreak();
	}
	if (InNode->Value.MeshesInBounds.GetCount() > 1)
	{
		uint32 BiggestAxis = InNode->Value.Bounds.GetBiggestAxis();
		if (BiggestAxis == 3)
		{
			BiggestAxis = CallDepth % 2;
		}
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
		CallDepth++;
		GenerateOctTreeImpl(LNode, CallDepth);
		CallDepth++;
		GenerateOctTreeImpl(RNode, CallDepth);
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
	Result.CalculateFromArray(reinterpret_cast<double3*>(Bounds.GetData()), Bounds.GetCount() * 2);
	return Result;
}
