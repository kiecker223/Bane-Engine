#pragma once

#include "Common/Types.h"


class LinearAllocator
{
public:

	LinearAllocator() : m_Start(nullptr), m_Current(nullptr), m_End(nullptr) 
	{
		int i = 0; 
		UNUSED(i);
	}

	inline LinearAllocator(uint32 ByteSize)
	{
		Initialize(ByteSize);
	}

	inline void Initialize(uint8* InStart, uint8* InEnd)
	{
		m_Start = InStart;
		m_Current = m_Start;
		m_End = InEnd;
	}

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

	inline uint32 GetNumBytesFree() const
	{
		return static_cast<uint32>(reinterpret_cast<ptrdiff_t>(m_End) - reinterpret_cast<ptrdiff_t>(m_Current));
	}

	inline uint32 GetNumBytes() const
	{
		return static_cast<uint32>(reinterpret_cast<ptrdiff_t>(m_End) - reinterpret_cast<ptrdiff_t>(m_Start));
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

	inline uint8* AllocateBytes(uint32 NumBytes)
	{
		if (reinterpret_cast<ptrdiff_t>(m_Current) + NumBytes >= reinterpret_cast<ptrdiff_t>(m_End))
		{
			__debugbreak();
			return nullptr;
		}
		uint8* Result = m_Current;
		m_Current = reinterpret_cast<uint8*>(reinterpret_cast<ptrdiff_t>(m_Current) + NumBytes);
		return Result;
	}

	inline uint8* GetCurrentPointer() const
	{
		return m_Current;
	}

	template<typename TReturnType>
	inline TReturnType* Allocate()
	{
		TReturnType* ReturnVal = reinterpret_cast<TReturnType*>(AllocateBytes(sizeof(TReturnType)));
		BANE_CHECK(ReturnVal != nullptr);
		new(ReturnVal) TReturnType();
		return ReturnVal;
	}

	template<typename TReturnType, class ...U>
	inline TReturnType* Allocate(U&&... Args)
	{
		TReturnType* ReturnVal = reinterpret_cast<TReturnType*>(AllocateBytes(sizeof(TReturnType)));
		BANE_CHECK(ReturnVal != nullptr);
		new(&ReturnVal) TReturnType(std::forward(Args));
		return ReturnVal;
	}

private:

	uint8* m_Start;
	uint8* m_Current;
	uint8* m_End;
};
