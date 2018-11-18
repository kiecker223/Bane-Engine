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

typedef struct RAY_HIT_INFO {
	PhysicsBody* Body;
	double3 Position;
	double3 Normal;
} RAY_HIT_INFO;

class PhysicsWorld
{
public:

	PhysicsWorld() : CurrentId(0), bRunningPhysicsSim(true), m_bUnlockedForRead(true), MessageQueue() 
	{
		UNUSED(1);
		UNUSED(2);
		int i = 0;
		i += 1;
		int b = i;
		UNUSED(b);
	}

	inline uint32 AddBody(PHYSICS_BODY_CREATE_INFO Info)
	{
		std::lock_guard<std::mutex> ScopedMutex(BodyAddMutex);
		Info.Handle = CurrentId;
		CurrentId++;
		PhysicsBody Body(Info);
		AddList.Add(Body);
		return Info.Handle;
	}

	bool CastRay(const PHYSICS_RAY& InRay, RAY_HIT_INFO& OutInfo);
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
	PhysicsUpdateBuffer UpdateBuffer;
	TArray<PhysicsBody> AddList;
	ApplicationToPhysicsQueue MessageQueue;
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

	void GenerateOctTreeImpl(TBinaryTree<OctTreeNode>::TNode* InNode, uint32& CallDepth);
	BoundingBox CalculateBoundsForMeshes(const TArray<uint32>& MeshHandles);

	
	TBinaryTree<OctTreeNode> m_OctTree;
	TArray<PhysicsBody> m_Bodies;
	bool m_bUnlockedForRead;
};



