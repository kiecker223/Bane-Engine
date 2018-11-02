#include "PhysicsWorld.h"
#include <iostream>

// Gravitational constant  G = (6.6747*10^-11)
// Gravitational equation F = G*((m1*m2)/r*r)


void PhysicsWorld::SpawnThread()
{
	PhysicsThread = std::move(std::thread([this]()
	{
		this->UpdatePhysics();
	}));
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

		if (MessageQueue.GetMessageCount() > 0)
		{
			auto* Message = MessageQueue.GetMessage(0);
			while (Message->pNext)
			{
				if (Message->bQuit)
				{
					bRunningPhysicsSim = false;
				}
				else
				{
					Message->Execute(m_Bodies[Message->BodyId]);
					Message = Message->pNext;
				}
			}
		}

		m_bUnlockedForRead = false;
		/*
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
		}
		*/
		UpdateBuffer.Bodies = m_Bodies;
		m_bUnlockedForRead = true;
		auto TimeTaken = Clock::now() - Start;
		std::chrono::nanoseconds SleepTime = (std::chrono::nanoseconds(16666667) - std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken));
		std::this_thread::sleep_for(SleepTime);
	}
	std::cout << "Quiting physics thread" << std::endl;
}
