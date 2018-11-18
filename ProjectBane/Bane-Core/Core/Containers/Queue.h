#pragma once

#include "LinkedList.h"

template<typename TType>
class TQueue
{
public:

	TQueue()
	{
		m_Data.Construct();
		m_Data.Size = 0;
	}

	~TQueue()
	{
		m_Data.RecursiveDelete();
	}

	inline void Enqueue(TType&& InVal)
	{
		m_Data.Add(InVal);
	}

	inline void Enqueue(const TType& InVal)
	{
		m_Data.Add(InVal);
	}

	inline TType Dequeue()
	{
		TType Result(m_Data.Tail->Value);
		auto* Next = m_Data.Tail->Next;
		delete m_Data.Tail;
		m_Data.Tail = Next;
		m_Data.Size--;
		return Result;
	}

	inline uint32 GetCount()
	{
		return m_Data.Size;
	}

	inline bool IsEmpty()
	{
		return GetCount() == 0;
	}

	inline void Empty()
	{
		m_Data.RecursiveDelete();
		m_Data.Construct();
	}

private:

	TLinkedList<TType> m_Data;

};
