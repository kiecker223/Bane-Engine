#pragma once

#include "Common/Types.h"


class LinearAllocator
{
public:

	inline void Initialize(uint32 ByteSize)
	{
		m_Current = new uint8[ByteSize];
		m_Start = m_Current;
		m_End = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Current) + ByteSize);
	}

	inline void Destroy()
	{
		delete[] m_Start;
		m_Start = nullptr;
		m_Current = nullptr;
		m_End = nullptr;
	}

	inline void Reset(uint32 NewByteSize = 0)
	{
		if (NewByteSize)
		{
			Destroy();
			Initialize(NewByteSize);
		}
		m_Current = m_Start;
	}

	template<typename TReturnType>
	inline TReturnType* Allocate()
	{
		if (reinterpret_cast<ptrdiff_t>(m_Current) + sizeof(TReturnType) >= reinterpret_cast<ptrdiff_t>(m_End))
		{
			return nullptr;
		}
		TReturnType* ReturnVal = reinterpret_cast<TReturnType*>(m_Current);
		new(ReturnVal) TReturnType();
		m_Current = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Current) + sizeof(TReturnType));
		return ReturnVal;
	}

	template<typename TReturnType, class ...U>
	inline TReturnType* Allocate(U&&... Args)
	{
		if (reinterpret_cast<ptrdiff_t>(m_Current) + sizeof(TReturnType) >= reinterpret_cast<ptrdiff_t>(m_End))
		{
			return nullptr;
		}
		TReturnType* ReturnVal = reinterpret_cast<TReturnType*>(m_Current);
		new(&ReturnVal) TReturnType(std::forward(Args));
		m_Current = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Current) + sizeof(TReturnType));
		return ReturnVal;
	}

private:

	uint8* m_Start;
	uint8* m_Current;
	uint8* m_End;
};
