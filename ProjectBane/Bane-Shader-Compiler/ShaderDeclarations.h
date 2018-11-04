#pragma once
#include "Common/Types.h"
#include <Core/Containers/Array.h>

typedef struct SHADER_BYTECODE {
	TArray<uint8> ByteCode;
} SHADER_BYTECODE;

enum CompilerFlags
{
	WithDebugInfo_NoOptimization,
	WithoutDebugInfo_Optimize
};

