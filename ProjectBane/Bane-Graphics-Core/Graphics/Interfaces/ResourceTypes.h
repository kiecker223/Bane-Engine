#pragma once

#include "Common.h"
#include "Core/Data/Format.h"


typedef enum ECONSTANT_BUFFER_BIND_FLAG : uint {
	CONSTANT_BUFFER_BIND_FLAG_DYNAMIC,
	CONSTANT_BUFFER_BIND_FLAG_STATIC
} ECONSTANT_BUFFER_BIND_FLAG;

typedef enum ETEXTURE_USAGE : uint {
	TEXTURE_USAGE_SHADER_RESOURCE = 0x00000001,
	TEXTURE_USAGE_UNORDERED_ACCESS = 0x00000010,
	TEXTURE_USAGE_RENDER_TARGET = 0x00000100,
	TEXTURE_USAGE_DEPTH_STENCIL = 0x00000200,
} ETEXTURE_USAGE;

__forceinline ETEXTURE_USAGE operator | (const ETEXTURE_USAGE Lhs, const ETEXTURE_USAGE Rhs)
{
	return (ETEXTURE_USAGE)((uint)Lhs | (uint)Rhs);
}

typedef enum EBUFFER_USAGE : uint {
	BUFFER_USAGE_GPU,
	BUFFER_USAGE_CPU, // Mappable, constant buffers
	BUFFER_USAGE_UPLOAD // Specifically for upload buffers
} EBUFFER_USAGE;

typedef struct SUBRESOURCE_DATA {
	void* Pointer;
	uint Height;
	uint Width;
	uint Depth;
	uint Step;
	uint Subresource;
} SUBRESOURCE_DATA;
