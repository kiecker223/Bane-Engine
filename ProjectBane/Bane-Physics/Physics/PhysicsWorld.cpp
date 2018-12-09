#include "PhysicsWorld.h"
#include <algorithm>
#include <iostream>



void PhysicsWorld::SpawnThread()
{
	m_OctTreeDepth = 1;
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

void FindSmallestBoxesIntersectedImpl(std::vector<PhysicsWorld::OctTreeType::TNode*>& OutNodes, PhysicsWorld::OctTreeType::TNode* LastNode, const PHYSICS_RAY& InRay)
{
	for (uint32 i = 0; i < 8; i++)
	{
		if (LastNode->Children[i])
		{
			if (IntersectsBox(LastNode->Children[i]->Value.Bounds, InRay)) 
			{
				// Does this have any child nodes?
				if (LastNode->Children[i]->Children[0])
				{
					FindSmallestBoxesIntersectedImpl(OutNodes, LastNode->Children[i], InRay);
				}
				// if not then add it to the list of boxes we intersected
				else
				{
					OutNodes.push_back(LastNode);
				}
			}
		}
	}
}

bool PhysicsWorld::CastRay(const PHYSICS_RAY& InRay, RAY_HIT_INFO& OutInfo)
{
	using TNode = OctTreeType::TNode;
	std::vector<TNode*> IntersectedNodes;
	// Find a more performant way to get this done
	std::lock_guard<std::mutex> ScopeLock(GenerateOctTreeMutex); 
	FindSmallestBoxesIntersectedImpl(IntersectedNodes, m_OctTree.Base, InRay);
	std::sort(IntersectedNodes.begin(), IntersectedNodes.end(), [InRay](const TNode* Lhs, const TNode* Rhs)
	{
		double DistanceLhs = length(Lhs->Value.Bounds.GetCenter() - InRay.Position);
		double DistanceRhs = length(Rhs->Value.Bounds.GetCenter() - InRay.Position);
		return DistanceLhs < DistanceRhs;
	});

	for (auto* Node : IntersectedNodes)
	{
		double LeastT = M_POSITIVE_INFINITY;
		uint32 HandleOfClosestObj = 0xffffffff;
		double3 Normal;
		if (Node->Value.MeshesInBounds.size() > 0)
		{
			for (auto& MeshId : Node->Value.MeshesInBounds)
			{
				auto& Body = GetBody(MeshId.Handle);
				double T = (Body.TestRayHit(InRay.Position, InRay.Direction, Normal));
				if (T < LeastT)
				{
					LeastT = T;
					HandleOfClosestObj = Body.Handle;
				}
			}
			if (LeastT > -1.)
			{
				OutInfo.Body = GetBody(HandleOfClosestObj);
				OutInfo.Position = InRay.Position + (InRay.Direction * LeastT);
				OutInfo.Normal = Normal;
				return true;
			}
		}
	}
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
		if (AddList.size() > 0)
		{
			std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
			for (auto& Body : AddList)
			{
				m_Bodies.push_back(Body);
			}
			AddList.clear();
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
					if (testSphereIntersection(Body.Position, Body.Sphere.Radius, OtherBody.Position, OtherBody.Sphere.Radius))
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
		if (SleepTime > std::chrono::nanoseconds(0))
		{
			std::this_thread::sleep_for(SleepTime);
		}
	}
	std::cout << "Quiting physics thread" << std::endl;
}

void PhysicsWorld::RegenerateOctTree()
{
	std::scoped_lock<std::mutex> ScopeGuard(GenerateOctTreeMutex);

	using TNode = OctTreeType::TNode;

	//m_OctTree.RecursivelyDelete();
	if (!m_OctTree.Base)
	{
		m_OctTree.Base = new OctTreeType::TNode();
	}
	BoundingBox EntireScene = GetEntireSceneBounds();
	
	std::vector<PhysicsBodyRef> PhysBodyRefs(m_Bodies.size());
	for (uint32 i = 0; i < m_Bodies.size(); i++)
	{
		PhysBodyRefs[i].Handle = i;
		PhysBodyRefs[i].Position = m_Bodies[i].Position;
	}
	
	m_OctTree.Base->Value.Bounds = EntireScene;
	m_OctTree.Base->Value.MainBody = { 0xffffffff, double3(0., 0., 0.) };
	m_OctTree.Base->Value.MeshesInBounds = PhysBodyRefs;

	for (uint32 i = 0; i < 8; i++)
	{
		if (!m_OctTree.Base->Children[i])
		{
			m_OctTree.Base->Children[i] = new TNode();
			m_OctTree.Base->Children[i]->Parent = m_OctTree.Base;
		}
		uint32 CallDepth = 0;
		GenerateOctTreeImpl(m_OctTree.Base, m_OctTree.Base->Children[i], i, CallDepth);
	}
}

void PhysicsWorld::GenerateOctTreeImpl(OctTreeType::TNode* ParentNode, OctTreeType::TNode* InNode, uint32 ChildIdx, uint32& CallDepth)
{
	using TNode = OctTreeType::TNode;
	BoundingBox LastBounds = ParentNode->Value.Bounds;
	double3 LastCenter = LastBounds.GetCenter();
	double3 LastExtents = LastBounds.GetExtents();
	double3 HalfExtents = LastExtents / 2.;
	BoundingBox NewBounds;

	switch (ChildIdx)
	{
		case 0:
		{
			NewBounds.Min = LastBounds.Min;
			NewBounds.Max = LastCenter;
		} break;
		case 1:
		{
			NewBounds.Min = double3(LastCenter.x, LastBounds.Min.y, LastBounds.Min.z);
			NewBounds.Max = LastCenter + double3(HalfExtents.x, 0., 0.);
		} break;
		case 2:
		{
			NewBounds.Min = double3(LastBounds.Min.xy, LastCenter.z);
			NewBounds.Max = LastCenter + double3(0., 0., HalfExtents.z);
		} break;
		case 3:
		{
			NewBounds.Min = double3(LastCenter.x, LastBounds.Min.y, LastCenter.z);
			NewBounds.Max = LastCenter + double3(HalfExtents.x, 0., HalfExtents.z);
		} break;
		case 4:
		{
			NewBounds.Min = double3(LastBounds.Min.x, LastCenter.y, LastBounds.Min.z);
			NewBounds.Max = LastCenter + double3(0., HalfExtents.y, 0.);
		} break;
		case 5:
		{
			NewBounds.Min = double3(LastCenter.x, LastCenter.y, LastBounds.Min.z);
			NewBounds.Max = LastCenter + double3(HalfExtents.x, HalfExtents.y, 0.);
		} break;
		case 6:
		{
			NewBounds.Min = double3(LastBounds.Min.x, LastCenter.y, LastCenter.z);
			NewBounds.Max = LastCenter + double3(0., HalfExtents.y, -HalfExtents.z);
		} break;
		case 7:
		{
			NewBounds.Min = LastCenter;
			NewBounds.Max = LastBounds.Max;
		} break;
	}
	InNode->Value.Bounds = NewBounds;

	for (uint32 i = 0; i < ParentNode->Value.MeshesInBounds.size(); i++)
	{

	}

	if (ChildIdx == 0)
	{
		CallDepth++;
	}

	if (CallDepth < m_OctTreeDepth)
	{
		for (uint32 i = 0; i < 8; i++)
		{
			if (!InNode->Children[i])
			{
				InNode->Children[i] = new TNode();
				InNode->Children[i]->Parent = InNode;
			}
			GenerateOctTreeImpl(InNode, InNode->Children[i], i, CallDepth);
		}
	}
	if (ChildIdx == 7)
	{
		CallDepth--;
		return;
	}
}


BoundingBox PhysicsWorld::CalculateBoundsForMeshes(const std::vector<uint32>& MeshHandles)
{
	BoundingBox Result;
	std::vector<BoundingBox> Bounds;
	for (auto& Handle : MeshHandles)
	{
		Bounds.push_back(m_Bodies[Handle].GetBounds());
	}
	Result.CalculateFromArray(reinterpret_cast<double3*>(Bounds.data()), static_cast<uint32>(Bounds.size() * 2));
	return Result;
}

BoundingBox PhysicsWorld::GetEntireSceneBounds()
{
	BoundingBox Result;
	std::vector<BoundingBox> Bounds;
	for (uint32 i = 0; i < m_Bodies.size(); i++)
	{
		Bounds.push_back(m_Bodies[i].GetBounds());
	}
	Result.CalculateFromArray(reinterpret_cast<double3*>(Bounds.data()), static_cast<uint32>(Bounds.size() * 2));
	return Result;
}




