#pragma once

#include "Common/Types.h"
#include "Array.h"

template<typename TType>
class TLinearAllocator
{


	TType* m_Start;
	TArray<ptrdiff_t> m_AllocatedOffsets;
};


