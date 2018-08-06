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
	VirtualBlockLinearAllocator(uint32 Size, ELINEAR_ALLOCATOR_FLAGS InFlags) { Initialize(Size, Flags); }

	~VirtualBlockLinearAllocator() { FreeBlock(); }

	inline void Initialize(uint32 Size, ELINEAR_ALLOCATOR_FLAGS Flags);
	inline void FreeBlock();

	inline void* Allocate(uint32 Size);

	inline uint32 GetSize() const;
	inline uint32 GetAmountAllocated() const;

	template<typename T>
	inline T* Alloc()
	{
		return (T*)Allocate(sizeof(T));
	}

	template<typename T>
	inline T* Alloc(uint32 Num)
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
	inline T* AllocInitialized(uint32 Count, TConstructorArgs&&... Args)
	{
		T* RetPtr = Alloc(Count);
		T* Ptr = RetPtr;
		for (uint32 i = 0; i < Count; i++)
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

void VirtualBlockLinearAllocator::Initialize(uint32 Size, ELINEAR_ALLOCATOR_FLAGS InFlags)
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

uint32 VirtualBlockLinearAllocator::GetSize() const
{
	return static_cast<uint32>(m_EndPointer - m_BasePointer);
}

uint32 VirtualBlockLinearAllocator::GetAmountAllocated() const
{
	return static_cast<uint32>(m_EndPointer - m_CurrentPointer);
}

void* VirtualBlockLinearAllocator::Allocate(uint32 Size)
{
	byte* RetPointer = m_CurrentPointer;
	m_CurrentPointer += Size;
	return RetPointer;
}
