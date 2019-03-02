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
					OutNodes.push_back(LastNode->Children[i]);
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
		double LeastT = std::numeric_limits<double>::infinity();
		uint32 HandleOfClosestObj = 0xffffffff;
		double3 Normal;
		if (Node->Value.MeshesInBounds.size() > 0)
		{
			for (auto& MeshId : Node->Value.MeshesInBounds)
			{
				auto& Body = GetBody(MeshId.Handle);
				double T = (Body.TestRayHit(InRay.Position, InRay.Direction, Normal));
				if (T < LeastT && T > -1.)
				{
					LeastT = T;
					HandleOfClosestObj = Body.Handle;
				}
			}
			if (LeastT > -1. && LeastT < std::numeric_limits<double>::infinity())
			{
				OutInfo.Body = GetBody(HandleOfClosestObj);
				OutInfo.Position = InRay.Position + (InRay.Direction * LeastT);
				OutInfo.Normal = Normal;
				DebugRayCastIntersectedNodes = IntersectedNodes;
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

bool CheckSweepingCollision(PhysicsBody& Src, PhysicsBody& Other, double MotionDelta, double OtherMotionDelta, double TSrc, double TDst, double& OutPercentToCollision)
{
	double3 VelocityDir = normalized(Src.Velocity);
	double3 OtherVelocityDir = normalized(Other.Velocity);

	double Distance = length(Src.Position - Other.Position);
	if (Distance < min(Src.Sphere.Radius, Other.Sphere.Radius))
	{
		return true;
	}

	if (TSrc == TDst)
	{
		return false;
	}

	BANE_CHECK(TDst > TSrc);
	double PercentCheck = TDst - TSrc;

	if (testSphereIntersection(Src.Position + (VelocityDir * (MotionDelta * TSrc)), MotionDelta * PercentCheck, Other.Position + (OtherVelocityDir * (MotionDelta * TSrc)), Other.Sphere.Radius))
	{
		if (PercentCheck <= 0.05)
		{
			OutPercentToCollision = TSrc;
			return true;
		}
		return CheckSweepingCollision(Src, Other, MotionDelta, OtherMotionDelta, TSrc, TDst * 0.5, OutPercentToCollision);
	}
	else if (testSphereIntersection(Src.Position + (VelocityDir * (MotionDelta * TDst)), MotionDelta * PercentCheck, Other.Position + (OtherVelocityDir * (MotionDelta * TDst)), Other.Sphere.Radius))
	{
		if (PercentCheck <= 0.05)
		{
			OutPercentToCollision = TDst;
			return true;
		}
		return CheckSweepingCollision(Src, Other, MotionDelta, OtherMotionDelta, (TDst - TSrc) * 0.5, TDst, OutPercentToCollision);
	}
	return false;
}

void PhysicsWorld::UpdatePhysics()
{
	// At the start lock the physics buffer
	BANE_CHECK(SetThreadAffinityMask(GetCurrentThread(), 1 << 1) != 0);
	while (bRunningPhysicsSim)
	{
		m_bUnlockedForRead = false;
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
		// Calculate gravity acceleration
		{
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

					double Force = M_GRAV_CONST * ((Body.Mass * OtherBody.Mass) / (DistanceFromBody * DistanceFromBody));
					if (Force > 0.0 && Force != std::numeric_limits<double>::infinity())
					{
						ForceDir *= Force;
						double3 AccelerationDir = ForceDir / Body.Mass;
						Body.Velocity += (AccelerationDir * (1. / 60.));
						if (isNan(Body.Velocity))
						{
							__debugbreak();
						}
					}

//					double MotionDelta = length(Body.Velocity);
//					double3 ToObject = OtherBody.Position - Body.Position;
//					double ObjectDistance = length(ToObject);
//
//					if (MotionDelta + Body.Sphere.Radius > ObjectDistance)
//					{
// 						double3 MotionDir = normalized(Body.Velocity);
//
//						double NumToSubdivideTest = std::ceil(MotionDelta / (Body.Sphere.Radius * 0.9));
//						uint32 NumToSubdivide = static_cast<uint32>(NumToSubdivideTest);
//
//						double OtherMotionDelta = length(OtherBody.Velocity);
//
// 						double TimeToCollsion;
// 						if (CheckSweepingCollision(Body, OtherBody, MotionDelta, OtherMotionDelta, 0., 1., TimeToCollsion))
// 						{
// 							Body.Velocity = double3();
// 						}
//					}

					Body.Position += Body.Velocity;
				}
			}
		}
		{
			UpdateBuffer.Bodies = m_Bodies;
		}
		m_bUnlockedForRead = true;
		auto TimeTaken = Clock::now() - Start;
		SecondsTakenForThread = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken).count()) / 1e9f;
		//std::cout << "Physics thread: " << SecondsTakenForThread << std::endl;
		std::chrono::nanoseconds SleepTime = (std::chrono::nanoseconds(16666667) - std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken));
		if (SleepTime > std::chrono::nanoseconds(0))
		{
			std::this_thread::sleep_for(SleepTime);
		}
	}
	std::cout << "Quiting physics thread" << std::endl;
}

void RecursivelyClearTreeImpl(PhysicsWorld::OctTreeType::TNode* Node)
{
	using TNode = PhysicsWorld::OctTreeType::TNode;
	for (uint32 i = 0; i < 8; i++)
	{
		if (Node->Children[i])
		{
			RecursivelyClearTreeImpl(Node->Children[i]);
		}
	}
	Node->Value.MeshesInBounds.clear();
}

void RecursivelyClearTree(PhysicsWorld::OctTreeType& OctTree)
{
	using TNode = PhysicsWorld::OctTreeType::TNode;
	RecursivelyClearTreeImpl(OctTree.Base);
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
	else
	{
		RecursivelyClearTree(m_OctTree);
	}
	BoundingBox EntireScene = GetEntireSceneBounds();
	
	std::vector<PhysicsBodyRef> PhysBodyRefs(m_Bodies.size());
	for (uint32 i = 0; i < m_Bodies.size(); i++)
	{
		PhysBodyRefs[i].Handle = i;
		PhysBodyRefs[i].Position = m_Bodies[i].Position;
		PhysBodyRefs[i].Bounds = m_Bodies[i].GetBounds();
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
		else
		{
			m_OctTree.Base->Children[i]->Value.MeshesInBounds.clear();
		}
		uint32 CallDepth = 0;
		GenerateOctTreeImpl(m_OctTree.Base, m_OctTree.Base->Children[i], i, CallDepth);
	}
}

static bool AllComponentsBigger(const double3& Lhs, const double3& Rhs)
{
	return (Lhs.x > Rhs.x && Lhs.y > Rhs.y && Lhs.z > Rhs.z);
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

	auto NewExtents = NewBounds.GetExtents();
	if (ParentNode->Value.MeshesInBounds.size() > 0)
	{
		auto& MeshesInBounds = ParentNode->Value.MeshesInBounds;
		for (uint32 i = 0; i < MeshesInBounds.size(); i++)
		{
			auto& MeshInBounds = MeshesInBounds[i];
			if (InNode->Value.Bounds.PointInBounds(MeshInBounds.Position))
			{
				InNode->Value.MeshesInBounds.push_back(MeshInBounds);
			}
			else
			{
 				if (AllComponentsBigger(MeshInBounds.Bounds.GetExtents(), NewBounds.GetExtents()))
 				{
 					InNode->Value.MeshesInBounds.push_back(MeshInBounds);
 				}
			}
		}
	}
	
	if (AllComponentsBigger(double3(2377000, 2377000, 2377000), NewExtents) || InNode->Value.MeshesInBounds.size() <= 1)
	{
		return;
	}

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




