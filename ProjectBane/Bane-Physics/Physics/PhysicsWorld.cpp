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
		if (AddList.size() > 0)
		{
			for (auto& Body : AddList)
			{
				Bodies.push_back(Body);
			}
			AddList.clear();
		}

		if (MessageList.size() > 0)
		{
			for (auto& Message : MessageList)
			{
				if (Message.bQuit)
				{
					bRunningPhysicsSim = false;
				}
			}
			MessageList.clear();
		}

		using Clock = std::chrono::high_resolution_clock;
		auto Start = Clock::now();
		m_bUnlockedForRead = false;
		for (auto& Body : Bodies)
		{
			for (auto& OtherBody : Bodies)
			{
				if (OtherBody.Handle == Body.Handle)
				{
					continue;
				}
				double3 ForceDir = OtherBody.Position - Body.Position;
				double DistanceFromBody = length(ForceDir);
				normalize(ForceDir);

				double Force = M_GRAV_CONST * ((Body.Mass * OtherBody.Mass) / (DistanceFromBody * DistanceFromBody));
				ForceDir *= Force;
				double3 AccelerationDir = ForceDir / Body.Mass;
				Body.Velocity += (AccelerationDir * (1. / 60.));
				if (length(Body.Velocity) > 0.0001)
					Body.Position += Body.Velocity;
			}
		}
		UpdateBuffer.Bodies = Bodies;
		m_bUnlockedForRead = true;
		auto TimeTaken = Clock::now() - Start;
		std::chrono::nanoseconds SleepTime = (std::chrono::nanoseconds(16667) - std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken));
		std::this_thread::sleep_for(SleepTime);
		std::cout << "Physics thread time: " << (static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(TimeTaken).count()) / 1E+9f) << std::endl;
	}
	std::cout << "Quiting physics thread" << std::endl;
}
