#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <vector>
#include <utility>
#include <functional>
#include <thread>


class PhysicsMessage
{
public:

	PhysicsMessage(uint32 InEntityIDSource, bool bInQuit) : EntityIDSource(InEntityIDSource), bQuit(bInQuit) { }

	uint32 EntityIDSource;
	bool bQuit;

	virtual void Execute(PhysicsBody& BodyToOperate)
	{
		UNUSED(BodyToOperate);
	}
};

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
	PhysicsUpdateBuffer UpdateBuffer;
	std::vector<PhysicsBody> Bodies;
	std::vector<PhysicsBody> AddList;
	std::vector<PhysicsMessage> MessageList;
	uint32 CurrentId;
	bool bRunningPhysicsSim;

private:

	bool m_bUnlockedForRead;
};



