#pragma once
#include "Common/Types.h"

template<typename T>
class HeapQueue
{
public:

	class LinkedNode
	{
	public:

		T* Pointer;

	};

	HeapQueue() :
		Elements(nullptr),
		Count(0),
		AllocCount(0)
	{
		Resize(2);
	}

	HeapQueue(uint32 InCount) :
		Elements(nullptr),
		Count(0),
		AllocCount(0)
	{
		Resize(InCount);
	}

	HeapQueue(const HeapQueue<T>& Other) 
	{
		while (Other.Count)
		{
			Push(Other.Pop());
		}
	}

	~HeapQueue()
	{
		delete[] Elements;
	}

	inline T Pop()
	{
		BANE_CHECK(Count > 0);
		T Result(Elements[Count - 1]);
		Elements[Count - 1].~T();
		Count--;
		return Result;
	}

	inline void Push(const T& Value)
	{
		MoveElements();
		if (Count + 1 > AllocCount)
		{
			Resize(AllocCount * 2);
		}
		Count++;
		new (&Elements[0]) T(Value);
	}

	inline void Resize(uint32 NewSize)
	{
		BANE_CHECK(NewSize != 0);
		T* NewElements = new T[NewSize];
		if (Elements)
		{
			for (uint32 i = 0; i < Count; i++)
			{
				new (&NewElements[i]) T(Elements[i]);
			}
			delete[] Elements;
		}
		AllocCount = NewSize;
		Elements = NewElements;
	}

	inline void MoveElements()
	{
		if (Count > 0)
		{
			for (int32 i = Count - 1; i > -1; i--)
			{
				new (&Elements[i]) T(Elements[i - 1]);
			}
		}
	}
	T* Elements;
	uint32 Count;
	uint32 AllocCount;
};
