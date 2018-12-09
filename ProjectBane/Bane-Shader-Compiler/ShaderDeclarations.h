#pragma once
#include "Common/Types.h"
#include <vector>

typedef struct SHADER_BYTECODE {
	std::vector<uint8> ByteCode;
} SHADER_BYTECODE;

enum CompilerFlags
{
	WithDebugInfo_NoOptimization,
	WithoutDebugInfo_Optimize
};

