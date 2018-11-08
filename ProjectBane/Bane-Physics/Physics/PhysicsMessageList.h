#pragma once

#include "PhysicsBody.h"
#include "PhysicsMessage.h"

class ApplicationToPhysicsQueue
{
public:

	ApplicationToPhysicsQueue() : m_Start(nullptr), m_Current(nullptr), m_Last(nullptr) { InitialAlloc(1024); }

	ApplicationToPhysicsQueue(uint32 InitialSize)
	{
		InitialAlloc(InitialSize);
	}

	inline uint32 GetCurrentBufferSize() const
	{
		return static_cast<uint32>(reinterpret_cast<ptrdiff_t>(m_Last) - reinterpret_cast<ptrdiff_t>(m_Start));
	}

	inline uint32 GetMessageCount() const
	{
		return m_AllocatedObjects.GetCount();
	}

	template<typename T>
	inline T* AllocMessage(uint32 ParentHandle)
	{
		T* Result = new (Alloc(sizeof(T))) T(ParentHandle);
		GetMessage(m_AllocatedObjects.GetCount() - 1)->pNext = Result;
		return Result;
	}

	template<typename T>
	inline T* AllocMessage(bool bQuit)
	{
		T* Result = new (Alloc(sizeof(T))) T(bQuit);
		GetMessage(static_cast<uint32>(m_AllocatedObjects.GetCount()) - 1)->pNext = Result;
		return Result;
	}

	void Reset();

	inline PhysicsMessage* GetMessage(uint32 Index)
	{
		return reinterpret_cast<PhysicsMessage*>(m_AllocatedObjects[Index] + reinterpret_cast<ptrdiff_t>(m_Start));
	}

private:

	void InitialAlloc(uint32 Size);
	void AllocateBiggerAndCopy(uint32 NewSize);

	uint8* Alloc(uint32 Size);

	TArray<ptrdiff_t> m_AllocatedObjects;
	uint8* m_Start;
	uint8* m_Current;
	uint8* m_Last;
};
