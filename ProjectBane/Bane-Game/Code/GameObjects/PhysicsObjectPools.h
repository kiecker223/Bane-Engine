#pragma once

#include "PhysicsPrimitives/CollisionPrimitives.h"
#include "PhysicsData.h"
#include <deque>


#define CreatePhysicsObjectPool(ClassName, TPhysicsObject) \
class ClassName \
{ \
public: \
	static void Initialize(); \
	static void Destroy(); \
	static ClassName& GetInstance(); \
	TPhysicsObject& AllocateShape(uint32& OutIndex); \
	void DeallocateShape(uint32 Index); \
private: \
	TPhysicsObject PhysicsObjectPool[32000]; \
	std::deque<uint32> AllocatedList; \
	std::deque<uint32> FreeList; \
	static ClassName* g_pPool; \
};

CreatePhysicsObjectPool(CollisionShapeObjectPool, CollisionShape)
CreatePhysicsObjectPool(PhysicsDataObjectPool, PhysicsData)

#undef CreatePhysicsObjectPool