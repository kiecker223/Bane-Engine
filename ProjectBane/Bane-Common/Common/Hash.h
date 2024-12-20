#pragma once

#include "Types.h"

namespace FNVConstants
{
	static constexpr uint32 FNV_BASIS = 2166136261;
	static constexpr uint32 FNV_PRIME = 16777619;
}

inline uint32 GetFNV0Hash(const char* Ptr, size_t Length)
{
	uint32 Result = FNVConstants::FNV_BASIS;
	for (size_t i = 0; i < Length; i++)
	{
		Result = Result * FNVConstants::FNV_PRIME;
		Result = Result ^ static_cast<uint32>(Ptr[i]);
	}
	return Result;
}


inline uint64 GetDJB264BitHash(const char* Ptr, size_t Length)
{
	uint64 Result = 5381;

	for (uint32 i = 0; i < Length; i++)
	{
		char Character = Ptr[i];
		Result = ((Result << 5) + Result) + Character;
	}

	return Result;
}

constexpr uint64 GetDJB264BitHash_ConstExpr_Impl(const char* String, uint64 InValue, uint32 Index)
{
	return String[Index] != 0 ? GetDJB264BitHash_ConstExpr_Impl(String, (((InValue << 5) + (uint64)InValue) + (uint64)String[Index]), Index + 1) : InValue;
}


constexpr uint64 GetDJB264BitHash_ConstExpr(const char* InString)
{
	return GetDJB264BitHash_ConstExpr_Impl(InString, 5381, 0);
}

