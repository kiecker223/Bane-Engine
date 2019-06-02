#include "PhysicsObjectPools.h"


CollisionShapeObjectPool* CollisionShapeObjectPool::g_pPool = nullptr;
PhysicsDataObjectPool* PhysicsDataObjectPool::g_pPool = nullptr;


#define DefinePhysicsObjectClass(ClassName, TPhysicsObject) \
void ClassName::Initialize()\
{ \
	ClassName::g_pPool = new ClassName(); \
} \
void ClassName::Destroy() \
{ \
	delete ClassName::g_pPool; \
} \
ClassName& ClassName::GetInstance() \
{ \
	return *ClassName::g_pPool; \
} \
TPhysicsObject& ClassName::AllocateShape(uint32& OutIndex) \
{ \
	OutIndex = FreeList.front(); \
	FreeList.pop_front(); \
	return PhysicsObjectPool[OutIndex]; \
} \
void ClassName::DeallocateShape(uint32 Index) \
{ \
	TPhysicsObject& PhysicsObject = PhysicsObjectPool[Index]; \
	memset(&PhysicsObject, 0, sizeof(TPhysicsObject)); \
	FreeList.push_front(Index); \
}

DefinePhysicsObjectClass(CollisionShapeObjectPool, CollisionShape)
DefinePhysicsObjectClass(PhysicsDataObjectPool, PhysicsData)

#undef DefinePhysicsObjectClass

