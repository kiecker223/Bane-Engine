#include "D3D12Translator.h"
#include "D3D12PipelineState.h"
#include <d3d12.h>
#include <DirectXColors.h>
#include <vector>

static std::vector<D3D12_BLEND> D3D12BlendTypes;
static std::vector<D3D12_BLEND_OP> D3D12BlendOperations;
static std::vector<D3D12_LOGIC_OP> D3D12LogicOperations;
static std::vector<D3D12_COMPARISON_FUNC> D3D12ComparisonFunctions;
static std::vector<D3D12_STENCIL_OP> D3D12StencilOperations;
static std::vector<D3D12_PRIMITIVE_TOPOLOGY_TYPE> D3D12PrimitiveTopologyTypes;

// This is an awful implementation of how to do filtering types, but its the fastest thing I can come up with
// Forget about the MIP_LINEAR and MIP_POINT, I know they are used for mipmaps but I'll be creating the mip maps myself
// Anisotropic blending - 
namespace D3D12Blends
{
	// @reference
	// -	https://msdn.microsoft.com/en-us/library/windows/desktop/dn770367(v=vs.85).aspx
	/*
		Takeaway
			Basically MINIMUM_MIN_XX_MAG_XX will return the smallest texel
			MAXIMUM_MIN_XX_MAG_XX will return the largest texel
			COMPARISON_MIN_XX_MAG_XX will linearly interpolate it, returning the average?

	*/
	/*
	D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT	= 0x10,
	D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x11,
	D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT	= 0x14,
	D3D12_FILTER_MIN_MAG_MIP_LINEAR	= 0x15,
	D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR	= 0x81,
	D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x84,
	D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR	= 0x85,
	D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT	= 0x90,
	D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x91,
	D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT	= 0x94,
	D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR	= 0x95,
	D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT	= 0x100,
	D3D12_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR	= 0x101,
	D3D12_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x104,
	D3D12_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR	= 0x105,
	D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT	= 0x110,
	D3D12_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x111,
	D3D12_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT	= 0x114,
	D3D12_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR	= 0x115,
	D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_POINT	= 0x180,
	D3D12_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR	= 0x181,
	D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x184,
	D3D12_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR	= 0x185,
	D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT	= 0x190,
	D3D12_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x191,
	D3D12_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT	= 0x194,
	D3D12_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR	= 0x195,
	*/

	//static std::vector<D3D12_BLEND> AvailableMagPoint;
	//static std::vector<D3D12_BLEND> AvailableMagLinear;
	//static std::vector<D3D12_BLEND> AvailableMinPoint;
	//static std::vector<D3D12_BLEND> AvailableMinLinear;
	//static std::vector<D3D12_BLEND> AvailableMinPointMipLinear;
	//static std::vector<D3D12_BLEND> AvailableMinPointMipPoint;
	//static std::vector<D3D12_BLEND> AvailableMinLinearMipLinear;
	//static std::vector<D3D12_BLEND> AvailableMinLinearMipPoint;
}

void InitializeD3D12Translator()
{
	{
		D3D12_BLEND Blends[19] = {
			D3D12_BLEND_ZERO,
			D3D12_BLEND_ONE,
			D3D12_BLEND_SRC_COLOR,
			D3D12_BLEND_INV_SRC_COLOR,
			D3D12_BLEND_SRC_ALPHA,
			D3D12_BLEND_INV_SRC_ALPHA,
			D3D12_BLEND_DEST_ALPHA,
			D3D12_BLEND_INV_DEST_ALPHA,
			D3D12_BLEND_DEST_COLOR,
			D3D12_BLEND_INV_DEST_COLOR,
			D3D12_BLEND_SRC_ALPHA_SAT,
			D3D12_BLEND_BLEND_FACTOR,
			D3D12_BLEND_INV_BLEND_FACTOR,
			D3D12_BLEND_SRC1_COLOR,
			D3D12_BLEND_INV_SRC1_COLOR,
			D3D12_BLEND_SRC1_ALPHA,
			D3D12_BLEND_INV_SRC1_ALPHA
		};

		D3D12BlendTypes.resize(19);
		memcpy((void*)D3D12BlendTypes.data(), Blends, sizeof(Blends));
	}

	{
		D3D12_BLEND_OP BlendOps[5] = {
			D3D12_BLEND_OP_ADD,
			D3D12_BLEND_OP_SUBTRACT,
			D3D12_BLEND_OP_REV_SUBTRACT,
			D3D12_BLEND_OP_MIN,
			D3D12_BLEND_OP_MAX
		};

		D3D12BlendOperations.resize(5);
		memcpy((void*)D3D12BlendOperations.data(), BlendOps, sizeof(BlendOps));
	}
	{
		D3D12_LOGIC_OP LogicOps[16] = {
			D3D12_LOGIC_OP_CLEAR,
			D3D12_LOGIC_OP_SET,
			D3D12_LOGIC_OP_COPY,
			D3D12_LOGIC_OP_COPY_INVERTED,
			D3D12_LOGIC_OP_NOOP,
			D3D12_LOGIC_OP_INVERT,
			D3D12_LOGIC_OP_AND,
			D3D12_LOGIC_OP_NAND,
			D3D12_LOGIC_OP_OR,
			D3D12_LOGIC_OP_NOR,
			D3D12_LOGIC_OP_XOR,
			D3D12_LOGIC_OP_EQUIV,
			D3D12_LOGIC_OP_AND_REVERSE,
			D3D12_LOGIC_OP_AND_INVERTED,
			D3D12_LOGIC_OP_OR_REVERSE,
			D3D12_LOGIC_OP_OR_INVERTED
		};

		D3D12LogicOperations.resize(16);
		memcpy((void*)D3D12LogicOperations.data(), LogicOps, sizeof(LogicOps));
	}
	{
		D3D12_COMPARISON_FUNC ComparisonFunctions[8] = {
			D3D12_COMPARISON_FUNC_NEVER,
			D3D12_COMPARISON_FUNC_LESS,
			D3D12_COMPARISON_FUNC_EQUAL,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_COMPARISON_FUNC_GREATER,
			D3D12_COMPARISON_FUNC_NOT_EQUAL,
			D3D12_COMPARISON_FUNC_GREATER_EQUAL,
			D3D12_COMPARISON_FUNC_ALWAYS
		};

		D3D12ComparisonFunctions.resize(8);
		memcpy((void*)D3D12ComparisonFunctions.data(), ComparisonFunctions, sizeof(ComparisonFunctions));
	}
	{
		D3D12_STENCIL_OP StencilOperations[8] = {
			D3D12_STENCIL_OP_KEEP,
			D3D12_STENCIL_OP_ZERO,
			D3D12_STENCIL_OP_REPLACE,	
			D3D12_STENCIL_OP_INCR_SAT,
			D3D12_STENCIL_OP_DECR_SAT,
			D3D12_STENCIL_OP_INVERT,
			D3D12_STENCIL_OP_INCR,
			D3D12_STENCIL_OP_DECR
		};

		D3D12StencilOperations.resize(8);
		memcpy((void*)D3D12StencilOperations.data(), StencilOperations, sizeof(StencilOperations));
	}
	{
		/*
		POLYGON_TYPE_POINTS
		POLYGON_TYPE_LINES,
		POLYGON_TYPE_TRIANGLES,
		POLYGON_TYPE_TRIANGLE_STRIPS
		*/
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologies[4] = {
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE  // Duplicate because d3d12 is weird about this
		};
		D3D12PrimitiveTopologyTypes.resize(4);
		memcpy((void*)D3D12PrimitiveTopologyTypes.data(), PrimitiveTopologies, sizeof(PrimitiveTopologies));
	}
	InitializeD3DCommonTranslator();
}

D3D12_BLEND D3D12_TranslateBlendType(EBLEND_STYLE Style)
{
	return D3D12BlendTypes[(uint32)Style];
}

D3D12_BLEND_OP D3D12_TranslateBlendOp(EBLEND_OP BlendOp)
{
	return D3D12BlendOperations[(uint32)BlendOp];
}

D3D12_LOGIC_OP D3D12_TranslateLogicOp(ELOGIC_OP InLogicOp)
{
	return D3D12LogicOperations[(uint32)InLogicOp];
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12_TranslatePolygonType(EPOLYGON_TYPE PolygonType)
{
	return D3D12PrimitiveTopologyTypes[(uint32)PolygonType];
}

D3D12_PRIMITIVE_TOPOLOGY D3D12_TranslatePrimitiveTopology(EPRIMITIVE_TOPOLOGY Topology)
{
	switch (Topology)
	{
	case PRIMITIVE_TOPOLOGY_LINES:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case PRIMITIVE_TOPOLOGY_POINTLIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PRIMITIVE_TOPOLOGY_TRIANGLESTRIPS:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D12_COMPARISON_FUNC D3D12_TranslateComparisonFunc(ECOMPARISON_FUNCTION InFunc)
{
	return D3D12ComparisonFunctions[(uint32)InFunc];
}

D3D12_STENCIL_OP D3D12_TranslateStencilOp(ESTENCIL_OP InOp)
{
	return D3D12StencilOperations[(uint32)InOp];
}

DXGI_FORMAT D3D12_TranslateDataFormat(EINPUT_ITEM_FORMAT Format)
{
	switch (Format)
	{
	case INPUT_ITEM_FORMAT_FLOAT: return DXGI_FORMAT_R32_FLOAT;
	case INPUT_ITEM_FORMAT_INT: return DXGI_FORMAT_R32_SINT;
	case INPUT_ITEM_FORMAT_FLOAT2: return DXGI_FORMAT_R32G32_FLOAT;
	case INPUT_ITEM_FORMAT_INT2: return DXGI_FORMAT_R32G32_SINT;
	case INPUT_ITEM_FORMAT_FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case INPUT_ITEM_FORMAT_INT3: return DXGI_FORMAT_R32G32B32_SINT;
	case INPUT_ITEM_FORMAT_FLOAT4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case INPUT_ITEM_FORMAT_INT4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

D3D12_INPUT_LAYOUT_DESC D3D12_TranslateInputLayout(const GFX_INPUT_LAYOUT_DESC& InDesc)
{
	D3D12_INPUT_LAYOUT_DESC Result;
	
	D3D12_INPUT_ELEMENT_DESC* ResultItems = new D3D12_INPUT_ELEMENT_DESC[InDesc.InputItems.size()]{ };

	for (uint32 i = 0; i < InDesc.InputItems.size(); i++)
	{
		const GFX_INPUT_ITEM_DESC& Item = InDesc.InputItems[i];
		ResultItems[i].SemanticName = new char[Item.Name.size()];
		memcpy((void*)ResultItems[i].SemanticName, Item.Name.c_str(), Item.Name.size() + 1);
		ResultItems[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ResultItems[i].InputSlot = 0;
		ResultItems[i].Format = D3D12_TranslateDataFormat(InDesc.InputItems[i].ItemFormat);
		ResultItems[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		ResultItems[i].InstanceDataStepRate = 0;
	}

	Result.pInputElementDescs = ResultItems;
	Result.NumElements = static_cast<uint32>(InDesc.InputItems.size());
	return Result;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12_TranslateGraphicsPipelineStateDesc(const GFX_PIPELINE_STATE_DESC* InDesc, D3D12ShaderSignature* OutSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC OutDesc = {};
	D3D12ShaderSignature ShaderSignature;
	{
		D3D12VertexShader* VertexShader = nullptr;
		D3D12PixelShader* PixelShader = nullptr;
		D3D12HullShader* HullShader = nullptr;
		D3D12GeometryShader* GeometryShader = nullptr;
		if (InDesc->VS)
		{
			VertexShader = dynamic_cast<D3D12VertexShader*>(InDesc->VS);
			if (VertexShader)
			{
				OutDesc.VS = VertexShader->GetShaderByteCode();
			}
		}
		if (InDesc->PS)
		{
			PixelShader = dynamic_cast<D3D12PixelShader*>(InDesc->PS);
			if (PixelShader)
			{
				OutDesc.PS = PixelShader->GetShaderByteCode();
			}
		}
		if (InDesc->HS)
		{
			HullShader = dynamic_cast<D3D12HullShader*>(InDesc->HS);
			if (HullShader)
			{
				OutDesc.HS = HullShader->GetShaderByteCode();
			}
		}
		if (InDesc->GS)
		{
			GeometryShader = dynamic_cast<D3D12GeometryShader*>(InDesc->GS);
			if (GeometryShader)
			{
				OutDesc.GS = GeometryShader->GetShaderByteCode();
			}
		}
		ShaderSignature = GetD3D12ShaderSignatureLibrary()->DetermineBestRootSignature(InDesc->Counts);
		*OutSignature = ShaderSignature;
		OutDesc.pRootSignature = ShaderSignature.RootSignature;
	}

	for (uint32 i = 0; i < InDesc->NumRenderTargets; i++)
	{
		OutDesc.BlendState.RenderTarget[i].BlendEnable	=			InDesc->RtvDescs[i].bBlendEnable;
		OutDesc.BlendState.RenderTarget[i].LogicOpEnable =			InDesc->RtvDescs[i].bLogicOpEnable;
		OutDesc.BlendState.RenderTarget[i].SrcBlend =				D3D12_TranslateBlendType(InDesc->RtvDescs[i].SrcBlend);
		OutDesc.BlendState.RenderTarget[i].DestBlend =				D3D12_TranslateBlendType(InDesc->RtvDescs[i].DstBlend);
		OutDesc.BlendState.RenderTarget[i].BlendOp =				D3D12_TranslateBlendOp(InDesc->RtvDescs[i].BlendOp);
		OutDesc.BlendState.RenderTarget[i].SrcBlendAlpha =			D3D12_TranslateBlendType(InDesc->RtvDescs[i].SrcBlendAlpha);
		OutDesc.BlendState.RenderTarget[i].DestBlendAlpha =			D3D12_TranslateBlendType(InDesc->RtvDescs[i].DstBlendAlpha);
		OutDesc.BlendState.RenderTarget[i].BlendOpAlpha =			D3D12_TranslateBlendOp(InDesc->RtvDescs[i].AlphaBlendOp);
		OutDesc.BlendState.RenderTarget[i].LogicOp =				D3D12_TranslateLogicOp(InDesc->RtvDescs[i].LogicOp);
		OutDesc.BlendState.RenderTarget[i].RenderTargetWriteMask =	D3D12_COLOR_WRITE_ENABLE_ALL;
		OutDesc.RTVFormats[i] = D3D_TranslateFormat(InDesc->RtvDescs[i].Format);
	}
	OutDesc.DSVFormat = D3D_TranslateFormat(InDesc->DepthStencilState->GetDesc().Format);
	OutDesc.SampleMask = 0xffffffff;
	OutDesc.NumRenderTargets = InDesc->NumRenderTargets;
	if (InDesc->RasterDesc.bFillSolid)
	{
		OutDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	}
	else
	{
		OutDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}

	if (InDesc->RasterDesc.bCull)
	{
		OutDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	}
	else
	{
		OutDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}

	OutDesc.RasterizerState.FrontCounterClockwise = InDesc->RasterDesc.bIsCounterClockwiseForward;
	OutDesc.RasterizerState.DepthBias = 0;
	OutDesc.RasterizerState.DepthBiasClamp = InDesc->RasterDesc.DepthBiasClamp;
	OutDesc.RasterizerState.DepthClipEnable = InDesc->RasterDesc.bDepthClipEnable;
	OutDesc.RasterizerState.MultisampleEnable = InDesc->RasterDesc.bMultisampleEnable;
	OutDesc.RasterizerState.ForcedSampleCount = 0;
	OutDesc.RasterizerState.AntialiasedLineEnable = InDesc->RasterDesc.bAntialiasedLineEnabled;

	if (InDesc->InputLayout)
	{
		D3D12InputLayout* InputLayout = dynamic_cast<D3D12InputLayout*>(InDesc->InputLayout);
		OutDesc.InputLayout = InputLayout->InputDesc;
	}

	if (InDesc->RasterDesc.bMultisampleEnable)
	{
		switch (InDesc->RasterDesc.MultisampleLevel)
		{
		case MULTISAMPLE_LEVEL_4X:
			OutDesc.RasterizerState.ForcedSampleCount = 4;
			break;
		case MULTISAMPLE_LEVEL_8X:
			OutDesc.RasterizerState.ForcedSampleCount = 8;
			break;
		case MULTISAMPLE_LEVEL_16X:
			OutDesc.RasterizerState.ForcedSampleCount = 16;
			break;
		default:
			OutDesc.RasterizerState.ForcedSampleCount = 0;
			break;
		}
	}

	OutDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	OutDesc.SampleDesc = D3D_TranslateMultisampleLevel(InDesc->RasterDesc.MultisampleLevel);
	OutDesc.PrimitiveTopologyType = D3D12_TranslatePolygonType(InDesc->PolygonType);

	if (InDesc->DepthStencilState)
	{
		GFX_DEPTH_STENCIL_DESC DepthStencilDesc = InDesc->DepthStencilState->GetDesc();

		if (DepthStencilDesc.bDepthEnable)
		{
			OutDesc.DepthStencilState.DepthEnable = DepthStencilDesc.bDepthEnable;
			OutDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			OutDesc.DepthStencilState.DepthFunc = D3D12_TranslateComparisonFunc(DepthStencilDesc.DepthFunction);
		}
		if (DepthStencilDesc.bStencilEnable)
		{
			OutDesc.DepthStencilState.StencilEnable = DepthStencilDesc.bStencilEnable;
			OutDesc.DepthStencilState.StencilReadMask = 0xff;
			OutDesc.DepthStencilState.StencilWriteMask = 0xff;
			D3D12_DEPTH_STENCILOP_DESC FrontFace;
			D3D12_DEPTH_STENCILOP_DESC BackFace;
			FrontFace.StencilFailOp = D3D12_TranslateStencilOp(DepthStencilDesc.FrontFace.StencilFailOp);
			FrontFace.StencilDepthFailOp = D3D12_TranslateStencilOp(DepthStencilDesc.FrontFace.StencilDepthFailOp);
			FrontFace.StencilPassOp = D3D12_TranslateStencilOp(DepthStencilDesc.FrontFace.StencilPassOp);
			FrontFace.StencilFunc =	D3D12_TranslateComparisonFunc(DepthStencilDesc.FrontFace.ComparisonFunction);

			BackFace.StencilFailOp = D3D12_TranslateStencilOp(DepthStencilDesc.BackFace.StencilFailOp);
			BackFace.StencilDepthFailOp = D3D12_TranslateStencilOp(DepthStencilDesc.BackFace.StencilDepthFailOp);
			BackFace.StencilPassOp = D3D12_TranslateStencilOp(DepthStencilDesc.BackFace.StencilPassOp);
			BackFace.StencilFunc = D3D12_TranslateComparisonFunc(DepthStencilDesc.BackFace.ComparisonFunction);

			OutDesc.DepthStencilState.FrontFace = FrontFace;
			OutDesc.DepthStencilState.BackFace = BackFace;
		}
	}
	return OutDesc;
}

D3D12_TEXTURE_ADDRESS_MODE D3D12_TranslateWrapMode(EWRAP_MODE WrapMode)
{
	switch (WrapMode)
	{
	case WRAP_MODE_LOOP:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case WRAP_MODE_CLAMP:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case WRAP_MODE_MIRROR:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	}
	return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
}


D3D12_SAMPLER_DESC D3D12_TranslateSamplerDesc(const SAMPLER_DESC& InDesc)
{
	D3D12_SAMPLER_DESC Result = { };
	Result.AddressU = D3D12_TranslateWrapMode(InDesc.UAddress);
	Result.AddressV = D3D12_TranslateWrapMode(InDesc.VAddress);
	Result.AddressW = D3D12_TranslateWrapMode(InDesc.WAddress);
	memcpy(Result.BorderColor, DirectX::Colors::White, sizeof(float) * 4);
	Result.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS;
	//Result.ComparisonFunc = D3D12_TranslateComparisonFunc(InDesc.ComparisonFunction);
	Result.MipLODBias = 0.0f;
	Result.MaxAnisotropy = 16;
	Result.MinLOD = 0.0f;
	Result.MaxLOD = D3D12_FLOAT32_MAX;

	if (InDesc.AnisotropicFiltering)
	{
		switch (InDesc.AnisotropicFiltering)
		{
			case ANISOTROPIC_FILTER_OVERRIDE_ANISOTROPIC:
			{
				Result.Filter = D3D12_FILTER_ANISOTROPIC;
			} break;
			case ANISOTROPIC_FILTER_OVERRIDE_MINIMUM_ANISOTROPIC:
			{
				Result.Filter = D3D12_FILTER_MINIMUM_ANISOTROPIC;
			} break;
			case ANISOTROPIC_FILTER_OVERRIDE_MAXIMUM_ANISOTROPIC:
			{
				Result.Filter = D3D12_FILTER_MAXIMUM_ANISOTROPIC;
			} break;
		}
		return Result;
	}
	
	assert(false); // NOT IMPLEMENTED
	return { };
}
