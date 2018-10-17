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
	ComponentAllocator(uint32 NumBytes);
	~ComponentAllocator();

	inline size_t GetNumBytesAllocated() const
	{
		return PtrEnd - PtrBegin;
	}

	inline size_t GetNumBytesUsed() const
	{
		return PtrCurrent - PtrBegin;
	}



	// Clears and dumps the current memory
	void Reserve(uint32 NumBytes);

	// Dumps the current memory, copys it over
	void ReserveAndCopy(size_t NumBytes);

	// Clears the component at a specified point and moves it
	// force move of all memory, or keep a free and allocated list?
	void RemovePointer(Component* Position);

	// This just handles putting the object in memory
	// all tracking is handled by the Entity object
	template<typename C>
	inline C* AllocateObject()
	{
		if (PtrCurrent + sizeof(C) > PtrEnd)
		{
			ReserveAndCopy(GetNumBytesAllocated() * 2);
			return AllocateObject<C>();
		}
		C* Pointer = new ((void*)PtrCurrent) C();
		AllocatedObjects.push_back(reinterpret_cast<Component*>(reinterpret_cast<ptrdiff_t>(Pointer) - reinterpret_cast<ptrdiff_t>(PtrBegin)));
		PtrCurrent += sizeof(C);
		return (C*)Pointer;
	}

	template<typename C, class... U>
	inline C* AllocateObjectCtor(U&&... Params)
	{
		if (PtrCurrent + sizeof(C) > PtrEnd)
		{
			ReserveAndCopy(GetNumBytesAllocated() * 2);
			return AllocateObject<C>(Params...);
		}
		C* Pointer = new ((void*)PtrCurrent) C(std::forward<U>(Params)...);
		AllocatedObjects.push_back(reinterpret_cast<Component*>(reinterpret_cast<ptrdiff_t>(Pointer) - reinterpret_cast<ptrdiff_t>(PtrBegin)));
		PtrCurrent += sizeof(C);
		return Pointer;
	}

	template<typename C>
	inline void RemovePointer(C* Position)
	{
		RemovePointer((C*)Position, sizeof(C));
	}

	inline void InternalFree()
	{
		for (auto* CompOffset : AllocatedObjects)
		{
			auto* c = reinterpret_cast<Component*>(reinterpret_cast<ptrdiff_t>(CompOffset) + reinterpret_cast<ptrdiff_t>(PtrBegin));
			c->~Component();
		}
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, (LPVOID)PtrBegin);
		PtrBegin = nullptr;
		PtrCurrent = nullptr;
		PtrEnd = nullptr;
	}

	inline bool InternalReAlloc(size_t ByteSize)
	{
		bool Result = true;
		size_t NumBytesUsed = GetNumBytesUsed();

		PtrBegin = (uint8*)HeapReAlloc(GetProcessHeap(), HEAP_NO_SERIALIZE, (LPVOID)PtrBegin, ByteSize);

		// Failed to realloc
		if (PtrBegin == nullptr)
		{
			return false;
		}

		PtrCurrent = PtrBegin + NumBytesUsed;
		PtrEnd = PtrBegin + ByteSize;

		return Result;
	}

	inline void InternalAllocate(size_t ByteSize)
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
	
	inline Component* GetComponent(uint32 Index)
	{
		return reinterpret_cast<Component*>(reinterpret_cast<ptrdiff_t>(AllocatedObjects[Index]) + reinterpret_cast<ptrdiff_t>(PtrBegin));
	}

	inline std::vector<Component*>& GetAllocatedObjects()
	{
		// std::lock_guard<std::mutex> LockGuard(ThreadLock);
		return AllocatedObjects;
	}
	
};