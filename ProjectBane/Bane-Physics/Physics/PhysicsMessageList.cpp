#include "PhysicsMessageList.h"


void ApplicationToPhysicsQueue::Reset()
{
	m_AllocatedObjects.clear();
	m_Current = m_Start;
}

void ApplicationToPhysicsQueue::InitialAlloc(uint32 Size)
{
	m_Start = new uint8[Size];
	m_Current = m_Start;
	m_Last = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Start) + static_cast<ptrdiff_t>(Size));
}

void ApplicationToPhysicsQueue::AllocateBiggerAndCopy(uint32 NewSize)
{
	uint8* OldPointer = m_Start;
	if (NewSize >= 0)
	{
		uint32 OldSize = GetCurrentBufferSize();
		InitialAlloc(NewSize);
		memcpy(m_Start, OldPointer, OldSize);
	}
	delete[] OldPointer;
}

uint8* ApplicationToPhysicsQueue::Alloc(uint32 Size)
{
	if (reinterpret_cast<ptrdiff_t>(m_Current) + static_cast<ptrdiff_t>(Size) > reinterpret_cast<ptrdiff_t>(m_Last))
	{
		AllocateBiggerAndCopy(GetCurrentBufferSize() * 2);
		return Alloc(Size);
	}

	uint8* Result = m_Current;
	m_Current = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Current) + static_cast<ptrdiff_t>(Size));
	m_AllocatedObjects.push_back(reinterpret_cast<ptrdiff_t>(m_Current) - reinterpret_cast<ptrdiff_t>(m_Start));
	return Result;
}
