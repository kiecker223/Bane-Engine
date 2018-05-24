#include "BaneObjectGeneralAllocator.h"
#include "Core/Systems/Logging/Logger.h"


BaneObjectGeneralAllocator::BaneObjectGeneralAllocator(uint NumBytes)
{
	InternalAllocate(NumBytes);
}

BaneObjectGeneralAllocator::~BaneObjectGeneralAllocator()
{
	InternalFree(); // Class is purely internal use, so go ahead and free it
}

void BaneObjectGeneralAllocator::Reserve(uint NumBytes)
{
	InternalFree();
	InternalAllocate(NumBytes);
}


void BaneObjectGeneralAllocator::ReserveAndCopy(uint NumBytes)
{
	uint8* PrevBegin = PtrBegin;
	uint8* PrevCurrent = PtrCurrent;
	uint8* PrevEnd = PtrEnd;
	uint PrevNumBytes = GetNumBytesUsed();

	// The behaviour of the realloc function here will not invalidate the previous pointer
	// I want to avoid the InternalFree function
	if (!InternalReAlloc(NumBytes))
	{
		InternalAllocate(NumBytes);
		PtrCurrent = PtrBegin + PrevNumBytes;
		PtrEnd = PtrBegin + NumBytes;
		for (uint i = 0; i < AllocatedObjects.size(); i++)
		{
			// This is a really dumb fix but essentially just set the pointer equal to the
			// position relative from the beginning that it used to be. COMPLETELY NOT THREAD SAFE
			ulong Diff = ((uint8*)AllocatedObjects[i]) - PrevBegin;
			AllocatedObjects[i] = (Component*)(PtrBegin + Diff);
		}
		memcpy(PtrBegin, PrevBegin, PrevNumBytes);
		HeapFree(GetProcessHeap(), 0, PrevBegin);
	}
}


void BaneObjectGeneralAllocator::RemovePointer(Component* Position)
{
	uint Size = Position->GetTypeSize();
	Position->~Component();
	uint8* Position_8Bit = (uint8*)Position;
	// The rationale here is to keep the memory contiguious, remove as few components as possible
	memcpy(Position_8Bit, Position_8Bit + Size, (uint)(PtrEnd - (Position_8Bit + Size))); 
	PtrEnd -= Size;

	int AllocatedIndex = -1;
	for (uint i = 0; i < AllocatedObjects.size(); i++)
	{
		if (AllocatedObjects[i] == Position) 
		{
			AllocatedIndex = i;
		}
	}

	if (AllocatedIndex >= 0)
	{
		AllocatedObjects.erase(AllocatedObjects.begin() + AllocatedIndex);
	}
}