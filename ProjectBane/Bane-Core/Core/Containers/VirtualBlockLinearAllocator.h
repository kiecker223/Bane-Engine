#pragma once

#include "Core/Common.h"

typedef enum ELINEAR_ALLOCATOR_FLAGS
{
	LINEAR_ALLOCATOR_NO_SERIALIZATION = 0x00000001
} ELINEAR_ALLOCATOR_FLAGS;

class VirtualBlockLinearAllocator
{

public:

	VirtualBlockLinearAllocator() { }
	VirtualBlockLinearAllocator(uint Size, ELINEAR_ALLOCATOR_FLAGS InFlags) { Initialize(Size, Flags); }

	~VirtualBlockLinearAllocator() { FreeBlock(); }

	inline void Initialize(uint Size, ELINEAR_ALLOCATOR_FLAGS Flags);
	inline void FreeBlock();

	inline void* Allocate(uint Size);

	inline uint GetSize() const;
	inline uint GetAmountAllocated() const;

	template<typename T>
	inline T* Alloc()
	{
		return (T*)Allocate(sizeof(T));
	}

	template<typename T>
	inline T* Alloc(uint Num)
	{
		return (T*)Allocate(sizeof(T) * Num);
	}

	template<typename T, class... TConstructorArgs>
	inline T* AllocInitialized(TConstructorArgs&&... Args)
	{
		T* RetPtr = Alloc();
		new (RetPtr) T(Args);
		return RetPtr;
	}

	template<typename T, class... TConstructorArgs>
	inline T* AllocInitialized(uint Count, TConstructorArgs&&... Args)
	{
		T* RetPtr = Alloc(Count);
		T* Ptr = RetPtr;
		for (uint i = 0; i < Count; i++)
		{
			new (Ptr++) T(Args);
		}
		return RetPtr;
	}

private:

	byte* m_BasePointer;
	byte* m_EndPointer;
	byte* m_CurrentPointer;	

};

void VirtualBlockLinearAllocator::Initialize(uint Size, ELINEAR_ALLOCATOR_FLAGS InFlags)
{
	DWORD Flags = 0;
	Flags |= MEM_COMMIT;
	Flags |= MEM_RESERVE;

	byte* BasePointer = static_cast<byte*>(VirtualAlloc(nullptr, Size, Flags, 0));

	m_BasePointer = BasePointer;
	m_CurrentPointer = BasePointer;
	m_EndPointer = BasePointer + Size;
}

void VirtualBlockLinearAllocator::FreeBlock()
{
	if (m_BasePointer != nullptr)
		VirtualFree(m_BasePointer, GetSize(), MEM_DECOMMIT);

	m_BasePointer = nullptr;
	m_CurrentPointer = nullptr;
	m_EndPointer = nullptr;
}

uint VirtualBlockLinearAllocator::GetSize() const
{
	return static_cast<uint>(m_EndPointer - m_BasePointer);
}

uint VirtualBlockLinearAllocator::GetAmountAllocated() const
{
	return static_cast<uint>(m_EndPointer - m_CurrentPointer);
}

void* VirtualBlockLinearAllocator::Allocate(uint Size)
{
	byte* RetPointer = m_CurrentPointer;
	m_CurrentPointer += Size;
	return RetPointer;
}
