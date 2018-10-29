#pragma once

class PhysicsBody;

class PhysicsMessage
{
public:

	PhysicsMessage() : pNext(nullptr) { }
	PhysicsMessage(uint32 InBodyId) : BodyId(InBodyId), bQuit(false), pNext(nullptr) { }
	PhysicsMessage(bool bInQuit) : bQuit(bInQuit), pNext(nullptr) { }

	uint32 BodyId;
	bool bQuit;

	virtual void Execute(PhysicsBody& BodyRef) { UNUSED(BodyRef); }

	PhysicsMessage* pNext;
};
