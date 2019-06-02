#pragma once

#include <Common.h>
#include <string.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>


inline bool IsAligned(uint64 Value, uint64 Alignment)
{
	return 0 == (Value & Alignment - 1);
}


inline void SIMDMemcpy(void* InDst, const void* InSrc, uint32 Size)
{
	BANE_CHECK(IsAligned(reinterpret_cast<uint64>(InDst), 16));
	BANE_CHECK(IsAligned(reinterpret_cast<uint64>(InSrc), 16));

	__m128i* pDst = reinterpret_cast<__m128i*>(InDst);
	const __m128i* pSrc = reinterpret_cast<const __m128i*>(InSrc);

	uint8* Dst = reinterpret_cast<uint8*>(InDst);
	const uint8* Src = reinterpret_cast<const uint8*>(InSrc);

	uint32 ExtraSize = Size % 16;
	uint32 ActualSize = Size - ExtraSize;

	for (uint32 i = 0; i < ActualSize / 16; i++)
	{
		_mm_stream_si128(&pDst[i], _mm_load_si128(&pSrc[i]));
	}

	if (ExtraSize > 0)
	{
		for (uint32 i = ActualSize; i < ActualSize + ExtraSize; i++)
		{
			Dst[i] = Src[i];
		}
	}
}

static int32 MEMSET_ZERO[4] = { 0, 0, 0, 0 };

inline void SIMDMemset(void* InDst, int32 Values[4], uint32 Size)
{
	BANE_CHECK(IsAligned(reinterpret_cast<uint64>(InDst), 16));
	uint32 ExtraSize = Size % 16;

	__m128i* Dst = reinterpret_cast<__m128i*>(InDst);
	__m128i Value = _mm_loadu_si128(reinterpret_cast<__m128i*>(Values));
	
	uint32 IterateCount = (Size - ExtraSize) / sizeof(__m128i);
	for (uint32 i = 0; i < IterateCount; i++)
	{
		_mm_stream_si128(&Dst[i], Value);
	}

	if (ExtraSize > 0)
	{
		uint8* pDst = reinterpret_cast<uint8*>(InDst);
		uint8* pSrc = reinterpret_cast<uint8*>(Values);
		for (uint32 i = Size - ExtraSize; i < Size; i++)
		{
			pDst[i] = pSrc[i];
		}
	}
}

#define SIMDMEMZERO(Dst, Size) SIMDMemset(Dst, MEMSET_ZERO, Size);