#pragma once

#include "Common.h"
#include <array>

template<typename TType, uint32 TMAX_SIZE>
class TStack
{
public:

	static const uint32 MAX_SIZE = TMAX_SIZE;
	typedef TStack<TType, MAX_SIZE> ThisType;

	TStack() : 
		NumUsed(0)
	{
	}

	TStack(ThisType& Rhs)
	{
		CopyFrom(Rhs);
	}

	inline uint32 MaxSize() const
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

	inline TType& operator [] (uint32 Position)
	{
		BANE_CHECK(Position < NumUsed);
		return Data[Position];
	}

	inline const TType& operator [] (uint32 Position) const
	{
		BANE_CHECK(Position < NumUsed);
		return Data[Position];
	}

	inline uint32 GetNumElements() const
	{
		return NumUsed;
	}

	// This is tricky because I can seriously fuck over some code by doing this wrong
	template<class TOther>
	inline ThisType AppendOther(const TOther& Other)
	{
		for (uint32 i = 0; i < Other.GetNumElements(); i++)
		{
			Push(Other[i]);
		}
		return *this;
	}

	template<class TOther>
	inline ThisType AppendToOther(TOther& Other)
	{
		for (uint32 i = 0; i < GetNumElements(); i++)
		{
			Other.Push(Data[i]);
		}
		return *this;
	}

	mutable uint32 NumUsed; // Gotta keep the const correctness
	std::array<TType, MAX_SIZE> Data;
};
