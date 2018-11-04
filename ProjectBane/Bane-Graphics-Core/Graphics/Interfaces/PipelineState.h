#pragma once
#include "Common.h"
#include "Core/Data/Format.h"
#include "../D3D12/D3D12Helper.h"
#include <Core/Containers/Array.h>


typedef enum ESHADER_STAGE {
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_PIXEL,
	SHADER_STAGE_HULL,
	SHADER_STAGE_GEOMETRY,
	SHADER_STAGE_COMPUTE = 0xffffffff
} ESHADER_STAGE;

class ShaderByteCode
{
public:
	TArray<uint8> ByteCode;
};

class IShader
{
public:
	virtual ~IShader() { }
};

class IVertexShader : public IShader
{
public:
	virtual ~IVertexShader() { }
};

class IPixelShader : public IShader 
{
public:
	virtual ~IPixelShader() { }
};

class IGeometryShader : public IShader
{
public:
	virtual ~IGeometryShader() { }
};

class IHullShader : public IShader
{
public:
	virtual ~IHullShader() { }
};

class IComputeShader : public IShader
{
public:
	virtual ~IComputeShader() { }
};

typedef enum ESHADER_PARAMETER_TYPE {
	SHADER_PARAMETER_TYPE_CBV,
	SHADER_PARAMETER_TYPE_SRV,
	SHADER_PARAMETER_TYPE_UAV
} ESHADER_PARAMETER_TYPE;

typedef enum EPOLYGON_TYPE {
	POLYGON_TYPE_POINTS,
	POLYGON_TYPE_LINES,
	POLYGON_TYPE_TRIANGLES,
	POLYGON_TYPE_TRIANGLE_STRIPS
} EPOLYGON_TYPE;

typedef enum ECOMPARISON_FUNCTION {
	COMPARISON_FUNCTION_NEVER,
	COMPARISON_FUNCTION_LESS,
	COMPARISON_FUNCTION_EQUAL,
	COMPARISON_FUNCTION_LESS_EQUAL,
	COMPARISON_FUNCTION_GREATER,
	COMPARISON_FUNCTION_NOT_EQUAL,
	COMPARISON_FUNCTION_GREATER_EQUAL,
	COMPARISON_FUNCTION_ALWAYS
} ECOMPARISON_FUNCTION;

typedef enum ESTENCIL_OP {
	STENCIL_OP_KEEP,
	STENCIL_OP_ZERO,
	STENCIL_OP_REPLACE,
	STENCIL_OP_INCR_SAT,
	STENCIL_OP_DECR_SAT,
	STENCIL_OP_INVERT,
	STENCIL_OP_INCR,
	STENCIL_OP_DECR
} ESTENCIL_OP;

typedef enum EBLEND_STYLE {
	BLEND_STYLE_ZERO,
	BLEND_STYLE_ONE,
	BLEND_STYLE_SRC_COLOR,
	BLEND_STYLE_INV_SRC_COLOR,
	BLEND_STYLE_SRC_ALPHA,
	BLEND_STYLE_INV_SRC_ALPHA,
	BLEND_STYLE_DEST_ALPHA,
	BLEND_STYLE_INV_DEST_ALPHA,
	BLEND_STYLE_DEST_COLOR,
	BLEND_STYLE_INV_DEST_COLOR,
	BLEND_STYLE_SRC_ALPHA_SAT,
	BLEND_STYLE_BLEND_FACTOR,
	BLEND_STYLE_INV_BLEND_FACTOR,
	BLEND_STYLE_SRC1_COLOR,
	BLEND_STYLE_INV_SRC1_COLOR,
	BLEND_STYLE_SRC1_ALPHA,
	BLEND_STYLE_INV_SRC1_ALPHA
} EBLEND_STYLE;

typedef enum EBLEND_OP {
	BLEND_OP_ADD,
	BLEND_OP_SUBTRACT,
	BLEND_OP_REV_SUBTRACT,
	BLEND_OP_MIN,
	BLEND_OP_MAX
} EBLEND_OP;

typedef enum ELOGIC_OP {
	LOGIC_OP_CLEAR,
	LOGIC_OP_SET,
	LOGIC_OP_COPY, 
	LOGIC_OP_COPY_INVERTED,
	LOGIC_OP_NOOP,
	LOGIC_OP_INVERT,
	LOGIC_OP_AND,
	LOGIC_OP_NAND,
	LOGIC_OP_OR,
	LOGIC_OP_NOR,
	LOGIC_OP_XOR,
	LOGIC_OP_EQUIV,
	LOGIC_OP_AND_REVERSE,
	LOGIC_OP_AND_INVERTED,
	LOGIC_OP_OR_REVERSE,
	LOGIC_OP_OR_INVERTED
} ELOGIC_OP;

typedef enum EMULTISAMPLE_LEVEL {
	MULTISAMPLE_LEVEL_0,
	MULTISAMPLE_LEVEL_4X,
	MULTISAMPLE_LEVEL_8X,
	MULTISAMPLE_LEVEL_16X
} EMULTISAMPLE_LEVEL;

enum {
	ENABLE_ALL_COLOR_WRITE = 15,
};

typedef enum EINITDEFAULT {
	INIT_DEFAULT
} EINITDEFAULT;

typedef struct GFX_RASTER_DESC {
	bool bFillSolid;
	bool bCull;
	bool bIsCounterClockwiseForward;
	bool bDepthClipEnable;
	bool bAntialiasedLineEnabled;
	bool bMultisampleEnable;
	float DepthBiasClamp;
	float SlopeScaledDepthBias;
	EMULTISAMPLE_LEVEL MultisampleLevel;
} GFX_RASTER_DESC;

inline GFX_RASTER_DESC CreateDefaultGFXRasterDesc()
{
	GFX_RASTER_DESC Result;
	Result.bFillSolid = true;
	Result.bCull = true;
	Result.bIsCounterClockwiseForward = false;
	Result.bDepthClipEnable = true;
	Result.bAntialiasedLineEnabled = false;
	Result.bMultisampleEnable = false;
	Result.DepthBiasClamp = 0.0f;
	Result.SlopeScaledDepthBias = 0.0f;
	return Result;
}

typedef struct GFX_DEPTH_STENCIL_OP_DESC {
	ESTENCIL_OP StencilFailOp;
	ESTENCIL_OP StencilDepthFailOp;
	ESTENCIL_OP StencilPassOp;
	ECOMPARISON_FUNCTION ComparisonFunction;
} GFX_DEPTH_STENCIL_OP_DESC;

typedef struct GFX_DEPTH_STENCIL_DESC {
	EFORMAT Format;
	bool bDepthEnable;
	uint32 DepthWriteMask;
	ECOMPARISON_FUNCTION DepthFunction;
	bool bStencilEnable;
	GFX_DEPTH_STENCIL_OP_DESC FrontFace;
	GFX_DEPTH_STENCIL_OP_DESC BackFace;
} GFX_DEPTH_STENCIL_DESC;

inline GFX_DEPTH_STENCIL_DESC CreateDefaultGFXDepthStencilDesc()
{
	GFX_DEPTH_STENCIL_DESC Result = { };
	Result.Format = FORMAT_D24_UNORM_S8_UINT;
	Result.bDepthEnable = true;
	Result.DepthWriteMask = 0xffffffff;
	Result.DepthFunction = COMPARISON_FUNCTION_LESS;
	Result.bStencilEnable = false;
	GFX_DEPTH_STENCIL_OP_DESC Desc = { STENCIL_OP_KEEP, STENCIL_OP_KEEP, STENCIL_OP_KEEP, COMPARISON_FUNCTION_LESS };
	Result.FrontFace = Desc;
	Result.BackFace = Desc;
	return Result;
}

typedef struct GFX_RENDER_TARGET_DESC {
	bool bBlendEnable;
	bool bLogicOpEnable;
	EBLEND_STYLE SrcBlend;
	EBLEND_STYLE DstBlend;
	EBLEND_OP BlendOp;
	EBLEND_STYLE SrcBlendAlpha;
	EBLEND_STYLE DstBlendAlpha;
	EBLEND_OP AlphaBlendOp;
	ELOGIC_OP LogicOp;
	EFORMAT Format;
} GFX_RENDER_TARGET_DESC;

inline GFX_RENDER_TARGET_DESC CreateDefaultGFXRenderTargetDesc()
{
	GFX_RENDER_TARGET_DESC Result = { };
	Result.bBlendEnable = false;
	Result.bLogicOpEnable = false;
	Result.SrcBlend = BLEND_STYLE_ONE;
	Result.DstBlend = BLEND_STYLE_ZERO;
	Result.BlendOp = BLEND_OP_ADD;
	Result.SrcBlendAlpha = BLEND_STYLE_ONE;
	Result.DstBlendAlpha = BLEND_STYLE_ZERO;
	Result.AlphaBlendOp = BLEND_OP_ADD;
	Result.LogicOp = LOGIC_OP_NOOP;
	Result.Format = FORMAT_R8G8B8A8_UNORM;
	return Result;
}

typedef enum EINPUT_ITEM_FORMAT {
	INPUT_ITEM_FORMAT_FLOAT,
	INPUT_ITEM_FORMAT_INT,
	INPUT_ITEM_FORMAT_FLOAT2,
	INPUT_ITEM_FORMAT_INT2,
	INPUT_ITEM_FORMAT_FLOAT3,
	INPUT_ITEM_FORMAT_INT3,
	INPUT_ITEM_FORMAT_FLOAT4,
	INPUT_ITEM_FORMAT_INT4
} EINPUT_ITEM_FORMAT;

inline uint32 TranslateItemFormatSize(EINPUT_ITEM_FORMAT Format)
{
	switch (Format)
	{
	case INPUT_ITEM_FORMAT_FLOAT:
	case INPUT_ITEM_FORMAT_INT:
		return 4;
	case INPUT_ITEM_FORMAT_FLOAT2:
	case INPUT_ITEM_FORMAT_INT2:
		return 8;
	case INPUT_ITEM_FORMAT_FLOAT3:
	case INPUT_ITEM_FORMAT_INT3:
		return 12;
	case INPUT_ITEM_FORMAT_FLOAT4:
	case INPUT_ITEM_FORMAT_INT4:
		return 16;
	}
	return 0;
}

typedef struct GFX_INPUT_ITEM_DESC {
	std::string Name;
	EINPUT_ITEM_FORMAT ItemFormat;
} GFX_INPUT_ITEM_DESC;

typedef struct GFX_INPUT_LAYOUT_DESC {
	TArray<GFX_INPUT_ITEM_DESC> InputItems;
} GFX_INPUT_LAYOUT_DESC;

inline uint32 GetLayoutDescSize(const GFX_INPUT_LAYOUT_DESC& InDesc)
{
	uint32 Result = 0;
	for (uint32 i = 0; i < InDesc.InputItems.GetElementCount(); i++)
	{
		Result += TranslateItemFormatSize(InDesc.InputItems[i].ItemFormat);
	}
	return Result;
}

class IInputLayout
{
public:
	virtual ~IInputLayout() { }

	virtual GFX_INPUT_LAYOUT_DESC GetDesc() const = 0;

};

inline uint32 GetLayoutStride(IInputLayout* Layout)
{
	return GetLayoutDescSize(Layout->GetDesc());
}

class IDepthStencilState
{
public:
	virtual ~IDepthStencilState() { }

	virtual const GFX_DEPTH_STENCIL_DESC GetDesc() const = 0;
};

typedef struct PIPELINE_STATE_RESOURCE_COUNTS {
	uint8 NumConstantBuffers, NumShaderResourceViews, NumSamplers, NumUnorderedAccessViews;
} PIPELINE_STATE_RESOURCE_COUNTS;

typedef struct GFX_PIPELINE_STATE_DESC {
	PIPELINE_STATE_RESOURCE_COUNTS Counts;
	IVertexShader* VS;
	IPixelShader* PS;
	IHullShader* HS;
	IGeometryShader* GS;
	IInputLayout* InputLayout;
	EPOLYGON_TYPE PolygonType;
	GFX_RASTER_DESC RasterDesc;
	bool bEnableAlphaToCoverage;
	bool bIndependentBlendEnable;
	GFX_RENDER_TARGET_DESC RtvDescs[8];
	IDepthStencilState* DepthStencilState;
	uint32 NumRenderTargets;


	GFX_PIPELINE_STATE_DESC()
	{
		ZeroMemory(this, sizeof(GFX_PIPELINE_STATE_DESC));
	}

	GFX_PIPELINE_STATE_DESC(EINITDEFAULT)
	{
		InitDefault();
	}

	void InitDefault()
	{
		ZeroMemory(this, sizeof(GFX_PIPELINE_STATE_DESC));
		PolygonType = POLYGON_TYPE_TRIANGLES;
		RasterDesc = CreateDefaultGFXRasterDesc();
		bEnableAlphaToCoverage = false;
		bIndependentBlendEnable = false;
		NumRenderTargets = 1;
		for (uint32 i = 0; i < 8; i++)
		{
			RtvDescs[i] = CreateDefaultGFXRenderTargetDesc();
		}
	}

} GFX_PIPELINE_STATE_DESC;

inline GFX_PIPELINE_STATE_DESC CreateDefaultGFXPipeline()
{
	GFX_PIPELINE_STATE_DESC Result(INIT_DEFAULT);
	return Result;
}

typedef struct COMPUTE_PIPELINE_STATE_DESC {
	PIPELINE_STATE_RESOURCE_COUNTS Counts;
	IComputeShader* CS;
} COMPUTE_PIPELINE_STATE_DESC;


class IPipelineState
{
public:

	virtual ~IPipelineState() { }

	virtual uint8* GetSerialized(uint32& OutNumBytes) const = 0;
};

class IGraphicsPipelineState : public IPipelineState
{
public:
	virtual ~IGraphicsPipelineState() { }
	virtual void GetDesc(GFX_PIPELINE_STATE_DESC* OutDesc) const = 0;
};

class IComputePipelineState : public IPipelineState
{
public:
	virtual ~IComputePipelineState() { }
	virtual void GetDesc(COMPUTE_PIPELINE_STATE_DESC* OutDesc) const = 0;
};

