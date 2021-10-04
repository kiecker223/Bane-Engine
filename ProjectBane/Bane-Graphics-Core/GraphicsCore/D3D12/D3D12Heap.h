#pragma once

#include <Common.h>
#include <d3d12.h>
#include <Core/Containers/LinearAllocator.h>
#include <deque>
#include <memory>


class ID3D12ManagedHeapObject
{
public:

	virtual void Initialize(ID3D12Heap* InHeap, ID3D12Device* pDevice, uint64 InAlignment, uint64 InHeapSize) = 0;

	virtual uint64 AllocateResource(const D3D12_RESOURCE_DESC& ResourceDesc) = 0;

	virtual ID3D12Heap* GetHeap() = 0;

	// This virtual method is completely different based on what kind of heap is being used.
	// In a linear heap allocator, this will get called at the end of each frames execution
	virtual void ResetHeap() = 0;

};

inline uint64 RoundUpToNextMultipleOf(uint64 Alignment, uint64 Number)
{
	// credit to https://github.com/BuckeyeSoftware/rex/blob/master/src/rx/core/memory/allocator.h#L47
	uint64 Dummy = (Number + (Alignment - 1)) & ~(Alignment - 1);
	return Dummy;
}

class D3D12LinearHeapAllocator : public ID3D12ManagedHeapObject
{
public:

	D3D12LinearHeapAllocator() { }
	void Initialize(ID3D12Heap* InHeap, ID3D12Device* pDevice, uint64 InAlignment, uint64 InHeapSize) override final
	{
		Device = pDevice;
		Heap = InHeap;
		Alignment = InAlignment;
		HeapSize = InHeapSize;
		CurrentPointer = 0;
	}

	uint64 AllocateResource(const D3D12_RESOURCE_DESC& ResourceDesc) override final
	{
		uint64 NewSize = RoundUpToNextMultipleOf(Alignment, HeapSize);
		uint64 Result = CurrentPointer;
		CurrentPointer += NewSize;
		return Result;
	}

	ID3D12Heap* GetHeap() override final 
	{
		return Heap;
	}

	void ResetHeap() override final
	{
		CurrentPointer = 0;
	}

	ID3D12Heap* Heap;
	ID3D12Device* Device;
	uint64 Alignment;
	uint64 HeapSize;
	uint64 CurrentPointer;

};

class D3D12Heap
{
public:

	struct D3D12HeapAllocation
	{
		uint64 Start;
		uint64 Size;
	};

	D3D12Heap() = delete;
	D3D12Heap(ID3D12Heap* InHeap, uint64 InAlignment, uint64 InHeapSize) :
		D3DHeap(InHeap),
		Alignment(InAlignment),
		HeapSize(InHeapSize)
	{
		FreeList.push_back({ 0, HeapSize });
	}

	~D3D12Heap()
	{
		//D3DHeap->Release();
	}

	inline uint64 AllocateResource(uint64 Size, uint64& OutSize)
	{
		if (Size == 0)
		{
			return 0;
		}
		BANE_CHECK(FreeList.size() > 0);

		using D3D12HeapAllocationIter = std::deque<D3D12HeapAllocation>::iterator;
		for (D3D12HeapAllocationIter Iter = FreeList.begin(); Iter != FreeList.end(); Iter++)
		{
			if (Iter->Size > Size)
			{
				auto Front = *Iter;
				D3D12HeapAllocation Allocation;
				uint64 OldSize = Front.Size;
				uint64 OldStart = Front.Start;
				uint64 NewSize = RoundUpToNextMultipleOf(Alignment, Size);
				uint64 NewStart = Front.Start + NewSize;
				Front.Start = NewStart;
				Front.Size = OldSize - NewSize;
				FreeList.erase(Iter);
				if (Front.Size > Alignment)
				{
					FreeList.push_back(Front);
				}
				Allocation.Start = OldStart;
				Allocation.Size = NewSize;
				AllocatedPointers.push_back(Allocation);
				OutSize = NewSize;
				return OldStart;
			}
			else if (Iter->Size == Size)
			{
				auto Front = *Iter;
				FreeList.erase(Iter);
				return Front.Start;
			}
		}
	}

	inline void DeAllocateResource(uint64 Pointer)
	{
		auto iter = std::find_if(AllocatedPointers.begin(), AllocatedPointers.end(), [&Pointer](const D3D12HeapAllocation& var)
			{
				if (var.Start == Pointer)
				{
					return true;
				}
				return false;
			}
		);

		if (iter != AllocatedPointers.end())
		{
			// So here as a workaround from potential bugs with fragmenting my heap
			// I'm not sure if I want to keep this behaviour but right now this is mostly for texture allocation so I think this should be fine
			// TODO:
			//		make this class an interface:
			//		class ID3D12UserHeap
			//		{
			//			virtual uint64 Allocate(uint64 Size) = 0;
			//			virtual void DeAllocate(uint64 Pointer) = 0;
			//		}
			// Potential benefits:
			//		* Can implement ring buffer behaviour
			//		* Other custom allocation behaviour
			//		* Not committed resources
			//		* Can use for other resources such as constant buffers and not have to force
			//		  a driver copy with cpu only constant buffers
			// However for now my implementation works
			uint64 EndLocation = iter->Start + iter->Size;
			// Maybe when I put this over to the directx renderer I'll implement the allocation slack when
			// we're allocating many megabytes for textures
			//const int64 ALLOCATION_SLACK = 32;
			using D3D12HeapAllocationListIter = std::deque<D3D12HeapAllocation>::iterator;
			for (D3D12HeapAllocationListIter FreeIter = FreeList.begin(); FreeIter != FreeList.end(); FreeIter++)
			{
				if (EndLocation >= FreeIter->Start && iter->Start < FreeIter->Start)
				{
					FreeIter->Start = iter->Start;
					FreeIter->Size += iter->Size;
					AllocatedPointers.erase(iter);
					return;
				}
			}
			// If we reach this point then it will have to be fragmented
			FreeList.push_back(*iter);
			AllocatedPointers.erase(iter);
		}
	}

	std::deque<D3D12HeapAllocation> FreeList;
	std::deque<D3D12HeapAllocation> AllocatedPointers;

	uint64 HeapSize;
	uint64 Alignment;
	ID3D12Heap* D3DHeap;
};