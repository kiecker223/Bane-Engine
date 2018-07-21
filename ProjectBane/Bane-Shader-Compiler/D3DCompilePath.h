#pragma once

#include "ShaderDeclarations.h"


SHADER_BYTECODE D3DCompileVertexShader(const std::string& InByteCode, CompilerFlags Flags);

SHADER_BYTECODE D3DCompilePixelShader(const std::string& InByteCode, CompilerFlags Flags);

SHADER_BYTECODE D3DCompileHullShader(const std::string& InByteCode, CompilerFlags Flags);

SHADER_BYTECODE D3DCompileGeometryShader(const std::string& InByteCode, CompilerFlags Flags);

SHADER_BYTECODE D3DCompileComputeShader(const std::string& InByteCode, CompilerFlags Flags);
