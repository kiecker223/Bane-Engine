#pragma once

#include "Common/Types.h"
#include "Array.h"

template<typename TType>
class TLinearAllocator
{


	TType* m_Start;
	std::vector<ptrdiff_t> m_AllocatedOffsets;
};


