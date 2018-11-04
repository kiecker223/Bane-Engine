#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <Core/Containers/Array.h>
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
	uint32 CurrentId;
	bool bRunningPhysicsSim;

private:

	TArray<PhysicsBody> m_Bodies;
	bool m_bUnlockedForRead;
};



