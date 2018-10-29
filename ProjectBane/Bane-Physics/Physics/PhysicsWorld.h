#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <vector>
#include <utility>
#include <functional>
#include <thread>
#include "PhysicsMessageList.h"





// Unused for the moment, fired to the entity system when, say a particular mesh was cut in half
class PhysicsToApplicationMessage
{
public:
};


class PhysicsWorld
{
public:

	PhysicsWorld() : CurrentId(0), bRunningPhysicsSim(true), m_bUnlockedForRead(true) { }

	inline uint32 AddBody(PhysicsBody& InBody)
	{
		InBody.Handle = CurrentId;
		CurrentId++;
		AddList.push_back(InBody);
		return InBody.Handle;
	}

	void SpawnThread();
	inline void DestroyThread()
	{
		PhysicsThread.join();
	}

	inline bool IsReadyForRead() const
	{
		return m_bUnlockedForRead;
	}


	void UpdatePhysics();
	
	std::thread PhysicsThread;
	ApplicationToPhysicsQueue MessageQueue;
	PhysicsUpdateBuffer UpdateBuffer;
	std::vector<PhysicsBody> Bodies;
	std::vector<PhysicsBody> AddList;
	
	uint32 CurrentId;
	bool bRunningPhysicsSim;

private:

	bool m_bUnlockedForRead;
};



