#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <vector>
#include <Core/Containers/Tree.h>
#include <utility>
#include <functional>
#include <thread>
#include <mutex>
#include "PhysicsMessageList.h"
#include <functional>




// Unused for the moment, fired to the entity system when, say a particular mesh was cut in half
class PhysicsToApplicationMessage
{
public:
};


typedef struct PHYSICS_RAY {
	double3 Position;
	double3 Direction;
	double MaxDistance;
} PHYSICS_RAY;

typedef struct RAY_HIT_INFO {
	PhysicsBody Body;
	double3 Position;
	double3 Normal;
} RAY_HIT_INFO;

class PhysicsWorld
{
public:

	PhysicsWorld() : CurrentId(0), bRunningPhysicsSim(true), m_bUnlockedForRead(true), MessageQueue() 
	{
	}

	inline uint32 AddBody(PHYSICS_BODY_CREATE_INFO Info)
	{
		std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
		Info.Handle = CurrentId;
		CurrentId++;
		PhysicsBody Body(Info);
		AddList.push_back(Body);
		return Info.Handle;
	}

	bool CastRay(const PHYSICS_RAY& InRay, RAY_HIT_INFO& OutInfo);
	void CastRayAtSpeedOfLight(const PHYSICS_RAY& InRay, std::function<PhysicsBody()>& HitFunc);
	void SpawnThread();
	inline void DestroyThread()
	{
	}

	inline bool IsReadyForRead()
	{
		bool bResult = m_bUnlockedForRead;
		if (bResult)
		{
			m_bUnlockedForRead = false;
			return true;
		}
		return false;
	}


	void UpdatePhysics();
	
	std::thread PhysicsThread;
	PhysicsUpdateBuffer UpdateBuffer;
	std::vector<PhysicsBody> AddList;
	ApplicationToPhysicsQueue MessageQueue;
	std::mutex BodyAddMutex;
	std::mutex GenerateOctTreeMutex;
	uint32 CurrentId;
	bool bRunningPhysicsSim;
	float SecondsTakenForThread;

	struct PhysicsBodyRef
	{
		uint32 Handle;
		double3 Position;
		BoundingBox Bounds;
	};

	struct OctTreeNode
	{
		OctTreeNode()
		{
		}

		OctTreeNode(const BoundingBox& InBounds, const PhysicsBodyRef& InMainBody, const std::vector<PhysicsBodyRef>& InBodies) :
			Bounds(InBounds),
			MainBody(InMainBody),
			MeshesInBounds(InBodies)
		{
		}

		BoundingBox Bounds;
		PhysicsBodyRef MainBody; // Given that there is only one mesh in these bounds, or that there will be submeshes in these bounds?
		std::vector<PhysicsBodyRef> MeshesInBounds;
	};

	
	using OctTreeType = TTree<OctTreeNode, 8>;

	inline OctTreeType& GetOctTree()
	{
		return m_OctTree;
	}

	PhysicsBody& GetBody(uint32 BodyId)
	{
		return m_Bodies[BodyId];
	}

	void RegenerateOctTree();

	std::vector<OctTreeType::TNode*> DebugRayCastIntersectedNodes;

private:

	void GenerateOctTreeImpl(OctTreeType::TNode* ParentNode, OctTreeType::TNode* InNode, uint32 ChildIdx, uint32& CallDepth);
	BoundingBox CalculateBoundsForMeshes(const std::vector<uint32>& MeshHandles);
	BoundingBox GetEntireSceneBounds();

	uint32 m_OctTreeDepth;
	OctTreeType m_OctTree;
	std::vector<PhysicsBody> m_Bodies;
	bool m_bUnlockedForRead;
};



