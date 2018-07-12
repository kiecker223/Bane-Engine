#pragma once
#include <d3d12.h>
#include "Graphics/Interfaces/PipelineState.h"
#include "Graphics/Interfaces/GraphicsCommandList.h"
#include "Core/Common.h"
#include "../D3DCommon/D3DCommonTranslator.h"
#include "D3D12ShaderSignatureLibrary.h"

// Allocs the memory needed for this to work
void InitializeD3D12Translator();

D3D12_BLEND D3D12_TranslateBlendType(EBLEND_STYLE Blend);

D3D12_BLEND_OP D3D12_TranslateBlendOp(EBLEND_OP BlendOp);

D3D12_LOGIC_OP D3D12_TranslateLogicOp(ELOGIC_OP LogicOp);

D3D12_PRIMITIVE_TOPOLOGY_TYPE D3D12_TranslatePolygonType(EPOLYGON_TYPE PolygonType);

D3D12_PRIMITIVE_TOPOLOGY D3D12_TranslatePrimitiveTopology(EPRIMITIVE_TOPOLOGY Topology);

D3D12_COMPARISON_FUNC D3D12_TranslateComparisonFunc(ECOMPARISON_FUNCTION InFunc);

D3D12_STENCIL_OP D3D12_TranslateStencilOp(ESTENCIL_OP InOp);

DXGI_FORMAT D3D12_TranslateDataFormat(EINPUT_ITEM_FORMAT Format);

D3D12_INPUT_LAYOUT_DESC D3D12_TranslateInputLayout(const GFX_INPUT_LAYOUT_DESC& InDesc);

D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12_TranslateGraphicsPipelineStateDesc(const GFX_PIPELINE_STATE_DESC* InDesc, D3D12ShaderSignature* OutSignature);

D3D12_SAMPLER_DESC D3D12_TranslateSamplerDesc(const SAMPLER_DESC& InDesc);