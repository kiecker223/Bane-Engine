#include "Allocator.h"
#include <malloc.h>


static HANDLE ProcessHeap;

void* operator new(size_t Size)
{
	return HeapAlloc(ProcessHeap, HEAP_GENERATE_EXCEPTIONS, Size);
}

void* operator new[](size_t Size)
{
	return HeapAlloc(ProcessHeap, HEAP_GENERATE_EXCEPTIONS, Size);
}

void operator delete(void* Ptr)
{
	HeapFree(ProcessHeap, 0, Ptr);
}

void operator delete[](void* Ptr)
{
	HeapFree(ProcessHeap, 0, Ptr);
}

namespace Memory
{
	void* Malloc(size_t Size)
	{
		HeapAlloc(ProcessHeap, HEAP_GENERATE_EXCEPTIONS, Size);
	}

	void MemFree(void** Ptr)
	{
		HeapFree(ProcessHeap, 0, *Ptr);
		*Ptr = nullptr;
	}

	void* MallocSerialized(size_t Size)
	{
		HeapAlloc(ProcessHeap, 0, Size);
	}
}



