#pragma once

#include "Core/Common.h"
#include <array>

template<typename TType, uint MAX_SIZE>
class StackQueue
{
public:

	typedef StackQueue<TType, MAX_SIZE> ThisType;

	StackQueue()
	{
	}

	StackQueue(ThisType& Rhs)
	{
		CopyFrom(Rhs);
	}

	ForceInline void Push(const TType& Value)
	{
		assert(NumUsed + 1 < MAX_SIZE);
		Data[NumUsed] = Value;
		NumUsed++;
	}

	ForceInline TType Pop()
	{
		TType Result(Data[NumUsed - 1]);
		NumUsed--;
		return Result;
	}

	ForceInline void Reset()
	{
		NumUsed = 0; // Just invalidate all the other data, we don't care for it
	}

	ForceInline void CopyTo(ThisType& Other)
	{
		Other.NumUsed = NumUsed;
		memcpy(Other.Data.data(), Data.data(), sizeof(TType) * Other.NumUsed);
	}

	ForceInline void CopyFrom(ThisType& Src)
	{
		Src.CopyTo(*this);
	}

	ForceInline TType& operator [] (uint Position)
	{
		assert(Position < NumUsed);
		return Data[Position];
	}

	ForceInline const TType& operator [] (uint Position) const
	{
		assert(Position < NumUsed);
		return Data[Position];
	}

	ForceInline uint GetNumElements() const
	{
		return NumUsed;
	}

	// This is tricky because I can seriously fuck over some code by doing this wrong
	template<class TOther>
	ForceInline ThisType AppendOther(const TOther& Other)
	{
		for (uint i = 0; i < Other.GetNumElements(); i++)
		{
			Push(Other[i]);
		}
		return *this;
	}

	template<class TOther>
	ForceInline ThisType AppendToOther(TOther& Other)
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
