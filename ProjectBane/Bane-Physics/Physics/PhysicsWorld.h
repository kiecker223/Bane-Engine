#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <Core/Containers/Array.h>
#include <Core/Containers/BinaryTree.h>
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

class PhysicsWorld
{
public:

	PhysicsWorld() : CurrentId(0), bRunningPhysicsSim(true), m_bUnlockedForRead(true) { }

	inline uint32 AddBody(PHYSICS_BODY_CREATE_INFO Info)
	{
		std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
		Info.Handle = CurrentId;
		CurrentId++;
		PhysicsBody Body(Info);
		AddList.Add(Body);
		return Info.Handle;
	}

	bool CastRay(const PHYSICS_RAY& InRay, PhysicsBody& HitBody);
	void CastRayAtSpeedOfLight(const PHYSICS_RAY& InRay, std::function<PhysicsBody()>& HitFunc);
	void SpawnThread();
	inline void DestroyThread()
	{
	}

	inline bool IsReadyForRead() const
	{
		return m_bUnlockedForRead;
	}


	void UpdatePhysics();
	
	std::thread PhysicsThread;
	ApplicationToPhysicsQueue MessageQueue;
	PhysicsUpdateBuffer UpdateBuffer;
	TArray<PhysicsBody> AddList;
	std::mutex BodyAddMutex;
	std::mutex GenerateOctTreeMutex;
	uint32 CurrentId;
	bool bRunningPhysicsSim;

	struct OctTreeNode
	{
		BoundingBox Bounds;
		TArray<uint32> MeshesInBounds;
	};

	TBinaryTree<OctTreeNode>& GetOctTree()
	{
		return m_OctTree;
	}

	PhysicsBody& GetBody(uint32 BodyId)
	{
		return m_Bodies[BodyId];
	}

	void RegenerateOctTree();

private:

	void GenerateOctTreeImpl(TBinaryTree<OctTreeNode>::TNode* InNode);
	BoundingBox CalculateBoundsForMeshes(const TArray<uint32>& MeshHandles);

	
	TBinaryTree<OctTreeNode> m_OctTree;
	TArray<PhysicsBody> m_Bodies;
	bool m_bUnlockedForRead;
};



