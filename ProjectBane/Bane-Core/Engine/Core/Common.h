#pragma once

#include <dxgiformat.h>
#include "BaneMacros.h"

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ulonglong;

typedef char		int8;
typedef short		int16;
typedef int			int32;
typedef long long	int64;

typedef uchar		uint8;
typedef ushort		uint16;
typedef uint		uint32;
typedef ulonglong	uint64;

typedef unsigned char byte;

#define ForceInline __forceinline
	
ForceInline uint64 GetDJB264BitHash(const char* Ptr, uint Length) 
{
	uint64 Result = 5381;

	for (uint i = 0; i < Length; i++)
	{
		char Character = Ptr[i];
		Result = ((Result << 5) + Result) + Character;
	}

	return Result;
}

//ForceInline uint64 GetDJB264BitHash(const std::string& Str)
//{
//	return GetDJB264BitHash(Str.c_str(), Str.length());
//}

constexpr uint64 GetDJB264BitHash_ConstExpr_Impl(const char* String, uint64 InValue, uint Index)
{
	return String[Index] != 0 ? GetDJB264BitHash_ConstExpr_Impl(String, (((InValue << 5) + InValue) + String[Index]), Index + 1) : InValue;
}

constexpr uint64 GetDJB264BitHash_ConstExpr(const char* InString)
{
	return GetDJB264BitHash_ConstExpr_Impl(InString, 5381, 0);
}

#ifdef _DEBUG
#define BANE_CHECK(x) if (!x) { __debugbreak(); abort(); }
#else
#define BANE_CHECK(x) x
#endif

inline uint GetDXGIFormatSize(DXGI_FORMAT Format)
{
	switch (Format)
	{
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_UNORM:
		return 1;
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_UNORM:
		return 2;
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return 4;
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_UINT:
		return 8;
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_SINT:
	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_UINT:
		return 12;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return 16;
	}
	return 0;
}


#ifdef _DEBUG
#include <comdef.h>
#include <sstream>
ForceInline std::string _BuildErrorMessage(uint Line, const char* File, const char* Function, HRESULT Res)
{
	std::stringstream Str;
	std::string ErrorMessage = _com_error(Res).ErrorMessage();
	Str << "Direct 3D error on line: " << Line << "\nin file: " << File << "\nin function: " << Function << "\nWith error code: " << std::to_string(Res);
	return Str.str();
}

#define D3D12ERRORCHECK(x) { HRESULT HResult = x; if (FAILED(HResult)) { MessageBoxA(nullptr, _BuildErrorMessage(__LINE__, __FILE__, __FUNCTION__, HResult).c_str(), "D3D12 ERROR:", MB_OK); __debugbreak(); } }
#else
#define D3D12ERRORCHECK(x) x
#endif
