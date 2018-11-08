#include "ComponentAllocator.h"
#include <Platform/System/Logging/Logger.h>


ComponentAllocator::ComponentAllocator(uint32 NumBytes)
{
	InternalAllocate(NumBytes);
}

ComponentAllocator::~ComponentAllocator()
{
	InternalFree(); // Class is purely internal use, so go ahead and free it
}

void ComponentAllocator::Reserve(uint32 NumBytes)
{
	InternalFree();
	InternalAllocate(NumBytes);
}


void ComponentAllocator::ReserveAndCopy(size_t NumBytes)
{
	uint8* PrevBegin = PtrBegin;
	size_t PrevNumBytes = GetNumBytesUsed();

	// The behaviour of the realloc function here will not invalidate the previous pointer
	// I want to avoid the InternalFree function
	if (!InternalReAlloc(NumBytes))
	{
		InternalAllocate(NumBytes);
		PtrCurrent = PtrBegin + PrevNumBytes;
		PtrEnd = PtrBegin + NumBytes;
		for (uint32 i = 0; i < AllocatedObjects.GetCount(); i++)
		{
			// This is a really dumb fix but essentially just set the pointer equal to the
			// position relative from the beginning that it used to be. COMPLETELY NOT THREAD SAFE
			int64 Diff = ((uint8*)AllocatedObjects[i]) - PrevBegin;
			AllocatedObjects[i] = (Component*)(PtrBegin + Diff);
		}
		memcpy(PtrBegin, PrevBegin, PrevNumBytes);
		HeapFree(GetProcessHeap(), 0, PrevBegin);
	}
}


void ComponentAllocator::RemovePointer(Component* Position)
{
	uint32 Size = Position->GetTypeSize();
	Position->~Component();
	uint8* Position_8Bit = (uint8*)Position;
	// The rationale here is to keep the memory contiguious, remove as few components as possible
	memcpy(Position_8Bit, Position_8Bit + Size, (uint32)(PtrEnd - (Position_8Bit + Size)));
	PtrEnd -= Size;

	int AllocatedIndex = -1;
	for (uint32 i = 0; i < AllocatedObjects.GetCount(); i++)
	{
		if (AllocatedObjects[i] == Position) 
		{
			AllocatedIndex = i;
		}
	}

	if (AllocatedIndex >= 0)
	{
		AllocatedObjects.RemoveAt(AllocatedIndex);
	}
}