#include "D3DCompilePath.h"
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <iostream>

#define failed(x) ((x) < 0)

uint32 ToD3DCompileFlags(CompilerFlags Flags)
{
	if (Flags == WithDebugInfo_NoOptimization)
	{
		return (D3DCOMPILE_OPTIMIZATION_LEVEL0 | D3DCOMPILE_DEBUG);
	}
	return 0;
}

SHADER_BYTECODE D3DCompileVertexShader(const std::string& InByteCode, CompilerFlags Flags)
{
	SHADER_BYTECODE Result;
	ID3DBlob* CompiledCode;
	ID3DBlob* ErrorMessage;	
	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			"VSMain",
			"vs_5_0",
			ToD3DCompileFlags(Flags), 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		std::cout << "Failed to compile shader: " << std::endl;
		std::cout << (const char*)ErrorMessage->GetBufferPointer() << std::endl;
		return Result;
	}
	Result.ByteCode.resize(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(Result.ByteCode.data(), CompiledCode->GetBufferPointer(), CompiledCode->GetBufferSize());
	CompiledCode->Release();
	return Result;
}

SHADER_BYTECODE D3DCompilePixelShader(const std::string& InByteCode, CompilerFlags Flags)
{
	SHADER_BYTECODE Result;
	ID3DBlob* CompiledCode;
	ID3DBlob* ErrorMessage;
	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			"PSMain",
			"ps_5_0",
			ToD3DCompileFlags(Flags), 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		std::cout << "Failed to compile shader: " << std::endl;
		std::cout << (const char*)ErrorMessage->GetBufferPointer() << std::endl;
		return Result;
	}
	Result.ByteCode.resize(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(Result.ByteCode.data(), CompiledCode->GetBufferPointer(), CompiledCode->GetBufferSize());
	CompiledCode->Release();
	return Result;
}

SHADER_BYTECODE D3DCompileHullShader(const std::string& InByteCode, CompilerFlags Flags)
{
	SHADER_BYTECODE Result;
	ID3DBlob* CompiledCode;
	ID3DBlob* ErrorMessage;
	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			"HSMain",
			"hs_5_0",
			ToD3DCompileFlags(Flags), 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		std::cout << "Failed to compile shader: " << std::endl;
		std::cout << (const char*)ErrorMessage->GetBufferPointer() << std::endl;
		return Result;
	}
	Result.ByteCode.resize(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(Result.ByteCode.data(), CompiledCode->GetBufferPointer(), CompiledCode->GetBufferSize());
	CompiledCode->Release();
	return Result;
}

SHADER_BYTECODE D3DCompileGeometryShader(const std::string& InByteCode, CompilerFlags Flags)
{
	SHADER_BYTECODE Result;
	ID3DBlob* CompiledCode;
	ID3DBlob* ErrorMessage;
	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			"GSMain",
			"gs_5_0",
			ToD3DCompileFlags(Flags), 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		std::cout << "Failed to compile shader: " << std::endl;
		std::cout << (const char*)ErrorMessage->GetBufferPointer() << std::endl;
		return Result;
	}
	Result.ByteCode.resize(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(Result.ByteCode.data(), CompiledCode->GetBufferPointer(), CompiledCode->GetBufferSize());
	CompiledCode->Release();
	return Result;
}

SHADER_BYTECODE D3DCompileComputeShader(const std::string& InByteCode, CompilerFlags Flags)
{
	SHADER_BYTECODE Result;
	ID3DBlob* CompiledCode;
	ID3DBlob* ErrorMessage;
	if (FAILED(
		D3DCompile(
		(void*)InByteCode.data(),
			InByteCode.size(),
			nullptr, nullptr, nullptr,
			"main",
			"cs_5_0",
			ToD3DCompileFlags(Flags), 0,
			&CompiledCode,
			&ErrorMessage)))
	{
		std::cout << "Failed to compile shader: " << std::endl;
		std::cout << (const char*)ErrorMessage->GetBufferPointer() << std::endl;
		return Result;
	}
	Result.ByteCode.resize(static_cast<uint32>(CompiledCode->GetBufferSize()));
	memcpy(Result.ByteCode.data(), CompiledCode->GetBufferPointer(), CompiledCode->GetBufferSize());
	CompiledCode->Release();
	return Result;

}
