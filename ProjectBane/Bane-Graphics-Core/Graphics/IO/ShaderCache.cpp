#include <thread>
#include "ShaderCache.h"
#include "Platform/System/Logging/Logger.h"
#include "Common.h"
#include "Platform/System/File/PlatformFile.h"
#include "../Interfaces/ApiRuntime.h"
#include "Common/StringHelpers.h"
#include <string>
#include <fstream>
#include <iostream>
#include <JSON/JsonCPP.h>



ShaderCache* ShaderCache::GInstance = nullptr;

IGraphicsPipelineState* ShaderCache::LoadGraphicsPipeline(const std::string& ShaderFile)
{
	auto PipeFind = m_GraphicsPipelines.find(ShaderFile);
	if (PipeFind == m_GraphicsPipelines.end())
	{
		// LOG??????
		__debugbreak();
		return nullptr;
	}
	return PipeFind->second;
}

IComputePipelineState* ShaderCache::LoadComputePipeline(const std::string& ShaderFile)
{
	auto PipeFind = m_ComputePipelines.find(ShaderFile);
	if (PipeFind == m_ComputePipelines.end())
	{
		__debugbreak();
		return nullptr;
	}
	return PipeFind->second;
}


void ShaderCache::ReloadAllShaders()
{

}

static EINPUT_ITEM_FORMAT ParseItemFormat(const std::string& ItemFormatStr)
{
#define CHECK_ITEMFORMAT(x) if (ItemFormatStr == #x) { return x; }

	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_FLOAT);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_INT);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_FLOAT2);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_INT2);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_FLOAT3);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_INT3);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_FLOAT4);
	CHECK_ITEMFORMAT(INPUT_ITEM_FORMAT_INT4);
	return INPUT_ITEM_FORMAT_FLOAT3;

#undef CHECK_ITEMFORMAT
}

bool ParseBool(std::string BoolStr)
{
	if (BoolStr == "true")
		return true;
	if (BoolStr == "false")
		return false;
	return false;
}

static EBLEND_STYLE ParseBlendStyle(const std::string& BlendStyleStr)
{
#define CHECK_BLEND_STYLE(x)  if (BlendStyleStr == #x) { return x; }

	CHECK_BLEND_STYLE(BLEND_STYLE_ZERO);
	CHECK_BLEND_STYLE(BLEND_STYLE_ONE);
	CHECK_BLEND_STYLE(BLEND_STYLE_SRC_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_SRC_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_SRC_ALPHA);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_SRC_ALPHA);
	CHECK_BLEND_STYLE(BLEND_STYLE_DEST_ALPHA);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_DEST_ALPHA);
	CHECK_BLEND_STYLE(BLEND_STYLE_DEST_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_DEST_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_SRC_ALPHA_SAT);
	CHECK_BLEND_STYLE(BLEND_STYLE_BLEND_FACTOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_BLEND_FACTOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_SRC1_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_SRC1_COLOR);
	CHECK_BLEND_STYLE(BLEND_STYLE_SRC1_ALPHA);
	CHECK_BLEND_STYLE(BLEND_STYLE_INV_SRC1_ALPHA);
	return (EBLEND_STYLE)0;

#undef CHECK_BLEND_STYLE
}

inline EFORMAT ParseFormat(const std::string& FormatStr)
{
#define CHECK_FORMAT(x) if (FormatStr == #x) { return x; }

	CHECK_FORMAT(FORMAT_UNKNOWN);
	CHECK_FORMAT(FORMAT_R32G32B32A32_TYPELESS);
	CHECK_FORMAT(FORMAT_R32G32B32A32_FLOAT);
	CHECK_FORMAT(FORMAT_R32G32B32A32_UINT);
	CHECK_FORMAT(FORMAT_R32G32B32A32_SINT);
	CHECK_FORMAT(FORMAT_R32G32B32_TYPELESS);
	CHECK_FORMAT(FORMAT_R32G32B32_FLOAT);
	CHECK_FORMAT(FORMAT_R32G32B32_UINT);
	CHECK_FORMAT(FORMAT_R32G32B32_SINT);
	CHECK_FORMAT(FORMAT_R16G16B16A16_TYPELESS);
	CHECK_FORMAT(FORMAT_R16G16B16A16_FLOAT);
	CHECK_FORMAT(FORMAT_R16G16B16A16_UNORM);
	CHECK_FORMAT(FORMAT_R16G16B16A16_UINT);
	CHECK_FORMAT(FORMAT_R16G16B16A16_SNORM);
	CHECK_FORMAT(FORMAT_R16G16B16A16_SINT);
	CHECK_FORMAT(FORMAT_R32G32_TYPELESS);
	CHECK_FORMAT(FORMAT_R32G32_FLOAT);
	CHECK_FORMAT(FORMAT_R32G32_UINT);
	CHECK_FORMAT(FORMAT_R32G32_SINT);
	CHECK_FORMAT(FORMAT_R32G8X24_TYPELESS);
	CHECK_FORMAT(FORMAT_D32_FLOAT_S8X24_UINT);
	CHECK_FORMAT(FORMAT_R32_FLOAT_X8X24_TYPELESS);
	CHECK_FORMAT(FORMAT_X32_TYPELESS_G8X24_UINT);
	CHECK_FORMAT(FORMAT_R10G10B10A2_TYPELESS);
	CHECK_FORMAT(FORMAT_R10G10B10A2_UNORM);
	CHECK_FORMAT(FORMAT_R10G10B10A2_UINT);
	CHECK_FORMAT(FORMAT_R11G11B10_FLOAT);
	CHECK_FORMAT(FORMAT_R8G8B8A8_TYPELESS);
	CHECK_FORMAT(FORMAT_R8G8B8A8_UNORM);
	CHECK_FORMAT(FORMAT_R8G8B8A8_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_R8G8B8A8_UINT);
	CHECK_FORMAT(FORMAT_R8G8B8A8_SNORM);
	CHECK_FORMAT(FORMAT_R8G8B8A8_SINT);
	CHECK_FORMAT(FORMAT_R16G16_TYPELESS);
	CHECK_FORMAT(FORMAT_R16G16_FLOAT);
	CHECK_FORMAT(FORMAT_R16G16_UNORM);
	CHECK_FORMAT(FORMAT_R16G16_UINT);
	CHECK_FORMAT(FORMAT_R16G16_SNORM);
	CHECK_FORMAT(FORMAT_R16G16_SINT);
	CHECK_FORMAT(FORMAT_R32_TYPELESS);
	CHECK_FORMAT(FORMAT_D32_FLOAT);
	CHECK_FORMAT(FORMAT_R32_FLOAT);
	CHECK_FORMAT(FORMAT_R32_UINT);
	CHECK_FORMAT(FORMAT_R32_SINT);
	CHECK_FORMAT(FORMAT_R24G8_TYPELESS);
	CHECK_FORMAT(FORMAT_D24_UNORM_S8_UINT);
	CHECK_FORMAT(FORMAT_R24_UNORM_X8_TYPELESS);
	CHECK_FORMAT(FORMAT_X24_TYPELESS_G8_UINT);
	CHECK_FORMAT(FORMAT_R8G8_TYPELESS);
	CHECK_FORMAT(FORMAT_R8G8_UNORM);
	CHECK_FORMAT(FORMAT_R8G8_UINT);
	CHECK_FORMAT(FORMAT_R8G8_SNORM);
	CHECK_FORMAT(FORMAT_R8G8_SINT);
	CHECK_FORMAT(FORMAT_R16_TYPELESS);
	CHECK_FORMAT(FORMAT_R16_FLOAT);
	CHECK_FORMAT(FORMAT_D16_UNORM);
	CHECK_FORMAT(FORMAT_R16_UNORM);
	CHECK_FORMAT(FORMAT_R16_UINT);
	CHECK_FORMAT(FORMAT_R16_SNORM);
	CHECK_FORMAT(FORMAT_R16_SINT);
	CHECK_FORMAT(FORMAT_R8_TYPELESS);
	CHECK_FORMAT(FORMAT_R8_UNORM);
	CHECK_FORMAT(FORMAT_R8_UINT);
	CHECK_FORMAT(FORMAT_R8_SNORM);
	CHECK_FORMAT(FORMAT_R8_SINT);
	CHECK_FORMAT(FORMAT_A8_UNORM);
	CHECK_FORMAT(FORMAT_R1_UNORM);
	CHECK_FORMAT(FORMAT_R9G9B9E5_SHAREDEXP);
	CHECK_FORMAT(FORMAT_R8G8_B8G8_UNORM);
	CHECK_FORMAT(FORMAT_G8R8_G8B8_UNORM);
	CHECK_FORMAT(FORMAT_BC1_TYPELESS);
	CHECK_FORMAT(FORMAT_BC1_UNORM);
	CHECK_FORMAT(FORMAT_BC1_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_BC2_TYPELESS);
	CHECK_FORMAT(FORMAT_BC2_UNORM);
	CHECK_FORMAT(FORMAT_BC2_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_BC3_TYPELESS);
	CHECK_FORMAT(FORMAT_BC3_UNORM);
	CHECK_FORMAT(FORMAT_BC3_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_BC4_TYPELESS);
	CHECK_FORMAT(FORMAT_BC4_UNORM);
	CHECK_FORMAT(FORMAT_BC4_SNORM);
	CHECK_FORMAT(FORMAT_BC5_TYPELESS);
	CHECK_FORMAT(FORMAT_BC5_UNORM);
	CHECK_FORMAT(FORMAT_BC5_SNORM);
	CHECK_FORMAT(FORMAT_B5G6R5_UNORM);
	CHECK_FORMAT(FORMAT_B5G5R5A1_UNORM);
	CHECK_FORMAT(FORMAT_B8G8R8A8_UNORM);
	CHECK_FORMAT(FORMAT_B8G8R8X8_UNORM);
	CHECK_FORMAT(FORMAT_R10G10B10_XR_BIAS_A2_UNORM);
	CHECK_FORMAT(FORMAT_B8G8R8A8_TYPELESS);
	CHECK_FORMAT(FORMAT_B8G8R8A8_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_B8G8R8X8_TYPELESS);
	CHECK_FORMAT(FORMAT_B8G8R8X8_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_BC6H_TYPELESS);
	CHECK_FORMAT(FORMAT_BC6H_UF16);
	CHECK_FORMAT(FORMAT_BC6H_SF16);
	CHECK_FORMAT(FORMAT_BC7_TYPELESS);
	CHECK_FORMAT(FORMAT_BC7_UNORM);
	CHECK_FORMAT(FORMAT_BC7_UNORM_SRGB);
	CHECK_FORMAT(FORMAT_AYUV);
	CHECK_FORMAT(FORMAT_Y410);
	CHECK_FORMAT(FORMAT_Y416);
	CHECK_FORMAT(FORMAT_NV12);
	CHECK_FORMAT(FORMAT_P010);
	CHECK_FORMAT(FORMAT_P016);
	CHECK_FORMAT(FORMAT_420_OPAQUE);
	CHECK_FORMAT(FORMAT_YUY2);
	CHECK_FORMAT(FORMAT_Y210);
	CHECK_FORMAT(FORMAT_Y216);
	CHECK_FORMAT(FORMAT_NV11);
	CHECK_FORMAT(FORMAT_AI44);
	CHECK_FORMAT(FORMAT_IA44);
	CHECK_FORMAT(FORMAT_P8);
	CHECK_FORMAT(FORMAT_A8P8);
	CHECK_FORMAT(FORMAT_B4G4R4A4_UNORM);
	CHECK_FORMAT(FORMAT_P208);
	CHECK_FORMAT(FORMAT_V208);
	CHECK_FORMAT(FORMAT_V408);
	CHECK_FORMAT(FORMAT_FORCE_UINT);
	return (EFORMAT)0;

#undef CHECK_FORMAT
}

static EBLEND_OP ParseBlendOp(const std::string& BlendOpStr)
{
#define CHECK_BLEND_OP(x) if (BlendOpStr == #x) { return x; }

	CHECK_BLEND_OP(BLEND_OP_ADD);
	CHECK_BLEND_OP(BLEND_OP_SUBTRACT);
	CHECK_BLEND_OP(BLEND_OP_REV_SUBTRACT);
	CHECK_BLEND_OP(BLEND_OP_MIN);
	CHECK_BLEND_OP(BLEND_OP_MAX);
	return (EBLEND_OP)0;

#undef CHECK_BLEND_OP
}

static ELOGIC_OP ParseLogicOp(const std::string& LogicOpStr)
{
#define CHECK_LOGIC_OP(x) if (LogicOpStr == #x) { return x; }

	CHECK_LOGIC_OP(LOGIC_OP_CLEAR);
	CHECK_LOGIC_OP(LOGIC_OP_SET);
	CHECK_LOGIC_OP(LOGIC_OP_COPY);
	CHECK_LOGIC_OP(LOGIC_OP_COPY_INVERTED);
	CHECK_LOGIC_OP(LOGIC_OP_NOOP);
	CHECK_LOGIC_OP(LOGIC_OP_INVERT);
	CHECK_LOGIC_OP(LOGIC_OP_AND);
	CHECK_LOGIC_OP(LOGIC_OP_NAND);
	CHECK_LOGIC_OP(LOGIC_OP_OR);
	CHECK_LOGIC_OP(LOGIC_OP_NOR);
	CHECK_LOGIC_OP(LOGIC_OP_XOR);
	CHECK_LOGIC_OP(LOGIC_OP_EQUIV);
	CHECK_LOGIC_OP(LOGIC_OP_AND_REVERSE);
	CHECK_LOGIC_OP(LOGIC_OP_AND_INVERTED);
	CHECK_LOGIC_OP(LOGIC_OP_OR_REVERSE);
	CHECK_LOGIC_OP(LOGIC_OP_OR_INVERTED);
	return (ELOGIC_OP)0;

#undef CHECK_LOGIC_OP
}

static ECOMPARISON_FUNCTION ParseComparisonFunction(const std::string& ComparisonFuncStr)
{
#define CHECK_COMPARISON_FUNC(x) if (ComparisonFuncStr == #x) { return x; }

	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_NEVER);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_LESS);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_EQUAL);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_LESS_EQUAL);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_GREATER);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_NOT_EQUAL);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_GREATER_EQUAL);
	CHECK_COMPARISON_FUNC(COMPARISON_FUNCTION_ALWAYS);
	return (ECOMPARISON_FUNCTION)0;

#undef CHECK_COMPARISON_FUNC
}

static ESTENCIL_OP ParseStencilOp(const std::string StencilOpStr)
{
#define CHECK_STENCIL_OP(x) if (StencilOpStr == #x) { return x; }

	CHECK_STENCIL_OP(STENCIL_OP_KEEP);
	CHECK_STENCIL_OP(STENCIL_OP_ZERO);
	CHECK_STENCIL_OP(STENCIL_OP_REPLACE);
	CHECK_STENCIL_OP(STENCIL_OP_INCR_SAT);
	CHECK_STENCIL_OP(STENCIL_OP_DECR_SAT);
	CHECK_STENCIL_OP(STENCIL_OP_INVERT);
	CHECK_STENCIL_OP(STENCIL_OP_INCR);
	CHECK_STENCIL_OP(STENCIL_OP_DECR);
	return (ESTENCIL_OP)0;

#undef CHECK_STENCIL_OP
}

static EMULTISAMPLE_LEVEL ParseMultisampleLevel(const std::string& MultisampleLevelStr)
{
#define CHECK_SAMPLELEVEL(x) if (MultisampleLevelStr == #x) { return x; }

	CHECK_SAMPLELEVEL(MULTISAMPLE_LEVEL_0);
	CHECK_SAMPLELEVEL(MULTISAMPLE_LEVEL_4X);
	CHECK_SAMPLELEVEL(MULTISAMPLE_LEVEL_8X);
	CHECK_SAMPLELEVEL(MULTISAMPLE_LEVEL_16X);
	return MULTISAMPLE_LEVEL_0;

#undef CHECK_SAMPLELEVEL
}

static EPOLYGON_TYPE ParsePolygonType(const std::string& PolygonTypeStr) 
{
#define CHECK_POLY(x) if (PolygonTypeStr == #x) { return x; }

	CHECK_POLY(POLYGON_TYPE_POINTS);
	CHECK_POLY(POLYGON_TYPE_LINES);
	CHECK_POLY(POLYGON_TYPE_TRIANGLES);
	CHECK_POLY(POLYGON_TYPE_TRIANGLE_STRIPS);
	return (EPOLYGON_TYPE)0;

#undef CHECK_POLY
}

using namespace nlohmann;


void ShaderCache::InitCache(const std::string& JsonLocation)
{
	json RootObject;
	{
		std::ifstream File(JsonLocation);

		File >> RootObject;
		File.close();
	}
	auto* Device = GetApiRuntime()->GetGraphicsDevice();
	for (auto RootIter = RootObject.begin(); RootIter != RootObject.end(); RootIter++)
	{
		PIPELINE_STATE_RESOURCE_COUNTS Counts;
		std::string CurrentPipeline = RootIter.key();
		json PipelineJson = RootIter.value();
		
		bool bIsGraphics = PipelineJson["IsGraphics"].get<bool>();

		Counts.NumConstantBuffers = static_cast<uint8>(PipelineJson["NumConstantBuffers"].get<uint32>());
		Counts.NumSamplers = static_cast<uint8>(PipelineJson["NumSamplers"].get<uint32>());
		Counts.NumShaderResourceViews = static_cast<uint8>(PipelineJson["NumShaderResourceViews"].get<uint32>());
		Counts.NumUnorderedAccessViews = static_cast<uint8>(PipelineJson["NumUnorderedAccessViews"].get<uint32>());

		if (bIsGraphics)
		{
			GFX_PIPELINE_STATE_DESC Desc(INIT_DEFAULT);
			Desc.Counts = Counts;
			std::string CompiledLocation = PipelineJson["ShaderReference"];
			// InputLayout
			{
				GFX_INPUT_LAYOUT_DESC CreatedDesc;
				json JsonInputLayoutDesc = PipelineJson["InputLayout"];
				for (auto I = JsonInputLayoutDesc.begin(); I != JsonInputLayoutDesc.end(); I++)
				{
					CreatedDesc.InputItems.push_back({ I.key(), ParseItemFormat(I.value().get<std::string>()) });
				}
				Desc.InputLayout = Device->CreateInputLayout(CreatedDesc);
			}
			// RasterDesc
			{
				json JsonRasterDesc = PipelineJson["RasterDesc"];
				GFX_RASTER_DESC CreatedDesc = { };
				CreatedDesc.bFillSolid					= JsonRasterDesc["bFillSolid"].get<bool>();
				CreatedDesc.bCull						= JsonRasterDesc["bCull"].get<bool>();
				CreatedDesc.bIsCounterClockwiseForward	= JsonRasterDesc["bIsCounterClockwiseForward"].get<bool>();
				CreatedDesc.bDepthClipEnable			= JsonRasterDesc["bDepthClipEnable"].get<bool>();
				CreatedDesc.bAntialiasedLineEnabled		= JsonRasterDesc["bAntialiasedLineEnabled"].get<bool>();
				CreatedDesc.bMultisampleEnable			= JsonRasterDesc["bMultisampleEnable"].get<bool>();
				CreatedDesc.DepthBiasClamp				= JsonRasterDesc["DepthBiasClamp"].get<float>();
				CreatedDesc.SlopeScaledDepthBias		= JsonRasterDesc["SlopedScaledDepthBias"].get<float>();
				CreatedDesc.MultisampleLevel			= ParseMultisampleLevel(JsonRasterDesc["MultisampleLevel"].get<std::string>());
				Desc.RasterDesc = CreatedDesc;
			}
			Desc.NumRenderTargets = PipelineJson["NumRenderTargets"].get<uint32>();
			json RtvDescs = PipelineJson["RtvDescs"];
			for (uint32 i = 0; i < Desc.NumRenderTargets; i++)
			{
				json RtvDesc = RtvDescs[i];
				GFX_RENDER_TARGET_DESC Rtv = { };
				Rtv.bBlendEnable	= RtvDesc["bBlendEnable"].get<bool>();
				Rtv.bLogicOpEnable	= RtvDesc["bLogicOpEnable"].get<bool>();
				Rtv.SrcBlend		= ParseBlendStyle(RtvDesc["SrcBlend"].get<std::string>());
				Rtv.DstBlend		= ParseBlendStyle(RtvDesc["DstBlend"].get<std::string>());
				Rtv.BlendOp			= ParseBlendOp(RtvDesc["BlendOp"].get<std::string>());
				Rtv.SrcBlendAlpha	= ParseBlendStyle(RtvDesc["SrcBlendAlpha"].get<std::string>());
				Rtv.DstBlendAlpha	= ParseBlendStyle(RtvDesc["DstBlendAlpha"].get<std::string>());
				Rtv.AlphaBlendOp	= ParseBlendOp(RtvDesc["AlphaBlendOp"].get<std::string>());
				Rtv.LogicOp			= ParseLogicOp(RtvDesc["LogicOp"].get<std::string>());
				Rtv.Format			= ParseFormat(RtvDesc["Format"].get<std::string>());
				Desc.RtvDescs[i] = Rtv;
			}
			// DepthStencilState
			{
				GFX_DEPTH_STENCIL_DESC DepthStencilState = { };
				json JsonDepthStencilState			= PipelineJson["DepthStencilState"];
				DepthStencilState.Format			= ParseFormat(JsonDepthStencilState["Format"].get<std::string>());
				DepthStencilState.bDepthEnable		= JsonDepthStencilState["bDepthEnable"].get<bool>();
				DepthStencilState.DepthWriteMask	= JsonDepthStencilState["DepthWriteMask"].get<uint32>();
				DepthStencilState.DepthFunction		= ParseComparisonFunction(JsonDepthStencilState["DepthFunction"].get<std::string>());
				DepthStencilState.bStencilEnable	= JsonDepthStencilState["bStencilEnable"].get<bool>();
				
				json JsonFrontFace = JsonDepthStencilState["FrontFace"];
				DepthStencilState.FrontFace.StencilFailOp		= ParseStencilOp(JsonFrontFace["StencilFailOp"].get<std::string>());
				DepthStencilState.FrontFace.StencilDepthFailOp	= ParseStencilOp(JsonFrontFace["StencilDepthFailOp"].get<std::string>());
				DepthStencilState.FrontFace.StencilPassOp		= ParseStencilOp(JsonFrontFace["StencilPassOp"].get<std::string>());
				DepthStencilState.FrontFace.ComparisonFunction	= ParseComparisonFunction(JsonFrontFace["ComparisonFunction"].get<std::string>());

				json JsonBackFace = JsonDepthStencilState["BackFace"];
				DepthStencilState.BackFace.StencilFailOp		= ParseStencilOp(JsonFrontFace["StencilFailOp"].get<std::string>());
				DepthStencilState.BackFace.StencilDepthFailOp	= ParseStencilOp(JsonFrontFace["StencilDepthFailOp"].get<std::string>());
				DepthStencilState.BackFace.StencilPassOp		= ParseStencilOp(JsonFrontFace["StencilPassOp"].get<std::string>());
				DepthStencilState.BackFace.ComparisonFunction	= ParseComparisonFunction(JsonFrontFace["ComparisonFunction"].get<std::string>());
				Desc.DepthStencilState = Device->CreateDepthStencilState(DepthStencilState);
			}

			Desc.PolygonType = ParsePolygonType(PipelineJson["PolygonType"].get<std::string>());
			Desc.bEnableAlphaToCoverage = PipelineJson["bEnableAlphaToCoverage"].get<bool>();
			Desc.bIndependentBlendEnable = PipelineJson["bIndependentBlendEnable"].get<bool>();

			struct ShaderHeader
			{
				uint32 PSStart, HSStart, GSStart;
			};
			ShaderHeader* pHeader;
			size_t BufferSize = GetFileSize(PipelineJson["ShaderReference"].get<std::string>());
			uint8* FileBinary = ReadFileBinary(PipelineJson["ShaderReference"].get<std::string>(), BufferSize);
			pHeader = reinterpret_cast<ShaderHeader*>(FileBinary);
			std::vector<uint8> VSByteCode;
			VSByteCode.resize(pHeader->PSStart - 78);
			memcpy(VSByteCode.data(), FileBinary + 33, VSByteCode.size());
			Desc.VS = Device->CreateVertexShaderFromBytecode(VSByteCode);
			// If we don't have a HullShader start location then we just assume that the shader ends at the end of the buffer
			size_t PSStart = pHeader->PSStart;
			size_t PSEnd = 0;
			if (pHeader->HSStart == 0)
			{
				PSEnd = BufferSize - 26;
			}
			else
			{
				PSEnd = pHeader->HSStart - 44;
			}

			std::vector<uint8> PSByteCode;
			PSByteCode.resize(PSEnd - PSStart);
			memcpy(PSByteCode.data(), FileBinary + static_cast<size_t>(PSStart), PSByteCode.size());
			Desc.PS = Device->CreatePixelShaderFromBytecode(PSByteCode);
			if (pHeader->HSStart != 0)
			{
				std::vector<uint8> HSByteCode;
				size_t HSEnd = 0;
				if (pHeader->GSStart == 0)
				{
					HSEnd = BufferSize - 24;
				}
				else
				{
					HSEnd = pHeader->GSStart - 48;
				}
				size_t HSSize = HSEnd - pHeader->HSStart;
				HSByteCode.resize(HSSize);
				memcpy(HSByteCode.data(), FileBinary + static_cast<size_t>(pHeader->HSStart), HSByteCode.size());
				Desc.HS = Device->CreateHullShaderFromBytecode(HSByteCode);
			}
			if (pHeader->GSStart != 0)
			{
				std::vector<uint8> GSByteCode;
				size_t GSEnd = BufferSize - 28;
				GSByteCode.resize(GSEnd - pHeader->GSStart);
				memcpy(GSByteCode.data(), FileBinary + static_cast<size_t>(pHeader->GSStart), GSEnd - pHeader->GSStart);
				Desc.GS = Device->CreateGeometryShaderFromBytecode(GSByteCode);
			}
			delete[] FileBinary;
			IGraphicsPipelineState* PipelineState = Device->CreatePipelineState(&Desc);
			auto PipelineNamePair = std::pair<std::string, IGraphicsPipelineState*>(CurrentPipeline, PipelineState);
			m_GraphicsPipelines.insert(PipelineNamePair);
		}
		else
		{
			COMPUTE_PIPELINE_STATE_DESC Desc;
			Desc.Counts = Counts;
			uint32 NumBytes = 0;
			uint8* FileBinary = ReadFileBinary(PipelineJson["ShaderReference"].get<std::string>(), NumBytes);
			std::vector<uint8> ByteCode(NumBytes);
			memcpy(ByteCode.data(), FileBinary, NumBytes);
			Desc.CS = Device->CreateComputeShaderFromBytecode(ByteCode);
			delete[] FileBinary;
			IComputePipelineState* PipelineState = Device->CreatePipelineState(&Desc);
			auto PipelineNamePair = std::pair<std::string, IComputePipelineState*>(CurrentPipeline, PipelineState);
			m_ComputePipelines.insert(PipelineNamePair);
		}
	}
}

void ShaderCache::DestroyCache()
{
	for (auto& Iter : m_GraphicsPipelines)
	{
		delete Iter.second;
	}
	m_GraphicsPipelines.clear();
	for (auto& Iter : m_ComputePipelines)
	{
		delete Iter.second;
	}
	m_ComputePipelines.clear();
}


