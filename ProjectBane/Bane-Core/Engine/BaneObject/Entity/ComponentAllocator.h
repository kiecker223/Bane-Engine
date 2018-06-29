#pragma once
#include <vector>
#include <mutex>
#include "ComponentBase.h"
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"


class ComponentAllocator
{
public:

	ComponentAllocator() : PtrBegin(nullptr), PtrCurrent(nullptr), PtrEnd(nullptr) { }
	ComponentAllocator(uint NumBytes);
	~ComponentAllocator();

	ForceInline uint GetNumBytesAllocated() const
	{
		return (uint)PtrEnd - (uint)PtrBegin;
	}

	ForceInline uint GetNumBytesUsed() const
	{
		return (uint)PtrCurrent - (uint)PtrBegin;
	}



	// Clears and dumps the current memory
	void Reserve(uint NumBytes);

	// Dumps the current memory, copys it over
	void ReserveAndCopy(uint NumBytes);

	// Clears the component at a specified point and moves it
	// force move of all memory, or keep a free and allocated list?
	void RemovePointer(Component* Position);

	// This just handles putting the object in memory
	// all tracking is handled by the Entity object
	template<typename C>
	ForceInline C* AllocateObject()
	{
		if (PtrCurrent + sizeof(C) > PtrEnd)
		{
			ReserveAndCopy(GetNumBytesAllocated() * 2);
			return AllocateObject<C>();
		}
		C* Pointer = new ((void*)PtrCurrent) C();
		AllocatedObjects.push_back(Pointer);
		PtrCurrent += sizeof(C);
		return (C*)Pointer;
	}

	template<typename C, class... U>
	ForceInline C* AllocateObjectCtor(U&&... Params)
	{
		if (PtrCurrent + sizeof(C) > PtrEnd)
		{
			ReserveAndCopy(GetNumBytesAllocated() * 2);
			return AllocateObject<C>(Params...);
		}
		C* Pointer = new ((void*)PtrCurrent) C(std::forward<U>(Params)...);
		AllocatedObjects.push_back(Pointer);
		PtrCurrent += sizeof(C);
		return Pointer;
	}

	template<typename C>
	ForceInline void RemovePointer(C* Position)
	{
		RemovePointer((C*)Position, sizeof(C));
	}

	ForceInline void InternalFree()
	{
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, (LPVOID)PtrBegin);
		for (auto* c : AllocatedObjects)
		{
			delete c;
		}
		PtrBegin = nullptr;
		PtrCurrent = nullptr;
		PtrEnd = nullptr;
	}

	ForceInline bool InternalReAlloc(uint ByteSize)
	{
		bool Result = true;
		uint NumBytesUsed = GetNumBytesUsed();
		uint8* PrevBegin = PtrBegin;

		PtrBegin = (uint8*)HeapReAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, (LPVOID)PtrBegin, ByteSize);

		// Failed to realloc
		if (PtrBegin == nullptr)
		{
			return false;
		}

		PtrCurrent = PtrBegin + NumBytesUsed;
		PtrEnd = PtrBegin + ByteSize;
		for (uint i = 0; i < AllocatedObjects.size(); i++)
		{
			ulong Diff = ((uint8*)AllocatedObjects[i]) - PrevBegin;
			AllocatedObjects[i] = (Component*)(PtrBegin + Diff);
		}

		return Result;
	}

	ForceInline void InternalAllocate(uint ByteSize)
	{
		PtrBegin = (uint8*)HeapAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, ByteSize);

		PtrCurrent = PtrBegin;
		PtrEnd = PtrBegin + ByteSize;
	}
	

	uint8* PtrBegin;
	uint8* PtrEnd;
	uint8* PtrCurrent;

	std::vector<Component*> AllocatedObjects;
	std::mutex ThreadLock;
	
	ForceInline std::vector<Component*>& GetAllocatedObjects()
	{
		// std::lock_guard<std::mutex> LockGuard(ThreadLock);
		return AllocatedObjects;
	}
	
};