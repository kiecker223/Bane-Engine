#pragma once
#include "Core/Common.h"
#include "ResourceTypes.h"


class IGPUResource
{
public:

	virtual ~IGPUResource() { }

	virtual void SetDebugName(const std::string& DebugName) = 0;
	virtual std::string GetDebugName() const = 0;
};


class IBuffer : public IGPUResource
{
public:
	virtual uint GetSizeInBytes() const = 0;
	virtual EBUFFER_USAGE GetUsage() const = 0;
};

typedef IBuffer IVertexBuffer;
typedef IBuffer IIndexBuffer;
typedef IBuffer IConstantBuffer;

class ITextureBase : public IGPUResource
{
public:
	virtual uint GetWidth() const = 0;
	virtual uint GetHeight() const = 0;
	virtual uint GetDepth() const = 0;
	virtual uint GetArraySize() const = 0;
	virtual ETEXTURE_USAGE GetUsage() const = 0;
	virtual EFORMAT GetFormat() const = 0;
	virtual uint GetMipCount() const = 0;
	ForceInline bool IsShaderResource() const { return GetUsage() & TEXTURE_USAGE_SHADER_RESOURCE; }
	ForceInline bool IsUnorderedAccess() const { return GetUsage() & TEXTURE_USAGE_UNORDERED_ACCESS; }
	ForceInline bool IsRenderTarget() const { return ((GetUsage() & TEXTURE_USAGE_RENDER_TARGET) == TEXTURE_USAGE_RENDER_TARGET); }
	ForceInline bool IsDepthStencil() const { return ((GetUsage() & TEXTURE_USAGE_DEPTH_STENCIL) == TEXTURE_USAGE_DEPTH_STENCIL); }
};

typedef ITextureBase ITexture2D;
typedef ITextureBase ITexture2DArray;
typedef ITextureBase ITexture3D;
typedef ITextureBase ITextureCube;


typedef enum EWRAP_MODE {
	WRAP_MODE_LOOP,
	WRAP_MODE_CLAMP,
	WRAP_MODE_MIRROR
} EWRAP_MODE;

typedef enum EMAG_FILTERING {
	MAG_FILTERING_POINT,
	MAG_FILTERING_LINEAR
} EMAG_FILTERING;

typedef enum EMIN_FILTERING {
	MIN_FILTERING_POINT,
	MIN_FILTERING_LINEAR,
	MIN_FILTERING_POINT_MIPMAP_POINT,
	MIN_FILTERING_POINT_MIPMAP_LINEAR,
	MIN_FILTERING_LINEAR_MIPMAP_POINT,
	MIN_FILTERING_LINEAR_MIPMAP_LINEAR,
} EMIN_FILTERING;

typedef enum EANISOTROPIC_FILTER_OVERRIDE {
	ANISOTROPIC_FILTER_OVERRIDE_NO_OVERRIDE = 0,
	ANISOTROPIC_FILTER_OVERRIDE_ANISOTROPIC,
	ANISOTROPIC_FILTER_OVERRIDE_MINIMUM_ANISOTROPIC,
	ANISOTROPIC_FILTER_OVERRIDE_MAXIMUM_ANISOTROPIC
} EANISOTROPIC_FILTER_OVERRIDE;

typedef struct SAMPLER_DESC {
	EWRAP_MODE UAddress;
	EWRAP_MODE VAddress;
	EWRAP_MODE WAddress;
	EMAG_FILTERING MagFilter;
	EMIN_FILTERING MinFilter;
	EANISOTROPIC_FILTER_OVERRIDE AnisotropicFiltering;
} SAMPLER_DESC;

inline SAMPLER_DESC CreateDefaultSamplerDesc()
{
	return { 
		WRAP_MODE_CLAMP, 
		WRAP_MODE_CLAMP, 
		WRAP_MODE_CLAMP,
		(EMAG_FILTERING)0, (EMIN_FILTERING)0, 
		ANISOTROPIC_FILTER_OVERRIDE_ANISOTROPIC 
	};
}

class ISamplerState
{
public:
	virtual	~ISamplerState() { }
	virtual SAMPLER_DESC GetDesc() const = 0;
};

