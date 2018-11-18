#pragma once

#include <Core/Containers/Queue.h>
#include <functional>
#include "PhysicsBody.h"
#include "PhysicsMessage.h"

typedef struct PHYSICS_MESSAGE_CALLBACK {
	std::function<void(PhysicsBody&)> Callback;
	uint32 Handle;
} PHYSICS_MESSAGE_CALLBACK;

class ApplicationToPhysicsQueue
{
public:
	
	ApplicationToPhysicsQueue()
	{
		UNUSED(1);
		UNUSED(2);
		int i = 0;
		i += 1;
		int b = i;
		UNUSED(b);
	}

	void AddCallback(const PHYSICS_MESSAGE_CALLBACK& Callback)
	{
		std::lock_guard<std::mutex> ScopedMutex(PushMessageMutex);
		Queue.Enqueue(Callback);
	}

	std::mutex PushMessageMutex;
	TQueue<PHYSICS_MESSAGE_CALLBACK> Queue;
private:

};
