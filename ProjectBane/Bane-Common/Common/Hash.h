#pragma once

#include "Types.h"

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

//ForceInline uint3264 GetDJB264BitHash(const std::string& Str)
//{
//	return GetDJB264BitHash(Str.c_str(), Str.length());
//}

constexpr uint64 GetDJB264BitHash_ConstExpr_Impl(const char* String, uint64 InValue, uint32 Index)
{
	return String[Index] != 0 ? GetDJB264BitHash_ConstExpr_Impl(String, (((InValue << 5) + InValue) + String[Index]), Index + 1) : InValue;
}

constexpr uint64 GetDJB264BitHash_ConstExpr(const char* InString)
{
	return GetDJB264BitHash_ConstExpr_Impl(InString, 5381, 0);
}
