#pragma once
#include "PhysicsBody.h"
#include "PhysicsUpdateBuffer.h"
#include <vector>


class PhysicsWorld
{
public:

	inline uint32 AddBody(PhysicsBody& InBody)
	{
		InBody.Handle = static_cast<uint32>(Bodies.size());
		Bodies.push_back(InBody);
		return InBody.Handle;
	}

	void UpdateBodies();
	
	PhysicsUpdateBuffer UpdateBuffer;
	std::vector<PhysicsBody> Bodies;
};



