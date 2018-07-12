#pragma once

#include "Core/Common.h"
#include <array>

template<typename TType, uint TMAX_SIZE>
class StackQueue
{
public:

	static const uint MAX_SIZE = TMAX_SIZE;
	typedef StackQueue<TType, MAX_SIZE> ThisType;

	StackQueue() : 
		NumUsed(0)
	{
	}

	StackQueue(ThisType& Rhs)
	{
		CopyFrom(Rhs);
	}

	inline uint MaxSize() const
	{
		return MAX_SIZE;
	}

	inline void Push(const TType& Value)
	{
		BANE_CHECK(NumUsed + 1 < MAX_SIZE);
		Data[NumUsed] = Value;
		NumUsed++;
	}

	inline TType Pop()
	{
		TType Result(Data[NumUsed - 1]);
		NumUsed--;
		return Result;
	}

	inline void Reset()
	{
		NumUsed = 0; // Just invalidate all the other data, we don't care for it
	}

	inline void CopyTo(ThisType& Other)
	{
		Other.NumUsed = NumUsed;
		memcpy(Other.Data.data(), Data.data(), sizeof(TType) * Other.NumUsed);
	}

	inline void CopyFrom(ThisType& Src)
	{
		Src.CopyTo(*this);
	}

	inline TType& operator [] (uint Position)
	{
		BANE_CHECK(Position < NumUsed);
		return Data[Position];
	}

	inline const TType& operator [] (uint Position) const
	{
		assert(Position < NumUsed);
		return Data[Position];
	}

	inline uint GetNumElements() const
	{
		return NumUsed;
	}

	// This is tricky because I can seriously fuck over some code by doing this wrong
	template<class TOther>
	inline ThisType AppendOther(const TOther& Other)
	{
		for (uint i = 0; i < Other.GetNumElements(); i++)
		{
			Push(Other[i]);
		}
		return *this;
	}

	template<class TOther>
	inline ThisType AppendToOther(TOther& Other)
	{
		for (uint i = 0; i < GetNumElements(); i++)
		{
			Other.Push(Data[i]);
		}
		return *this;
	}

	mutable uint NumUsed; // Gotta keep the const correctness
	std::array<TType, MAX_SIZE> Data;
};
