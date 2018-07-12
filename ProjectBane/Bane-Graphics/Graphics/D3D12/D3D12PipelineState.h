#pragma once

#include "../Interfaces/PipelineState.h"
#include "../D3DCommon/D3DCommon.h"
#include "Core/Types.h"
#include "D3D12ShaderSignature.h"

template<ESHADER_STAGE ShaderStage>
class NO_VTABLE D3D12Shader
{
public:
	static const ESHADER_STAGE Stage = ShaderStage;

	D3D12Shader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews) :
		ByteCode(InByteCode),
		CompiledCode(InCompiledCode),
		NumConstantBuffers(InNumConstantBuffers),
		NumShaderResourceViews(InNumShaderResourceViews),
		NumSamplers(InNumSamplers),
		NumUnorderedAccessViews(InNumUnorderedAccessViews)
	{
	}

	inline D3D12_SHADER_BYTECODE GetShaderByteCode() const
	{
		D3D12_SHADER_BYTECODE Ret = { };
		Ret.pShaderBytecode = CompiledCode.data();
		Ret.BytecodeLength = CompiledCode.size();
		return Ret;
	}

	std::string ByteCode;
	std::vector<uint8> CompiledCode;
	uint NumConstantBuffers;
	uint NumShaderResourceViews;
	uint NumSamplers;
	uint NumUnorderedAccessViews;
};

class D3D12VertexShader : public IVertexShader, public D3D12Shader<SHADER_STAGE_VERTEX>
{
public:
	D3D12VertexShader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews)
		: D3D12Shader(InByteCode, InCompiledCode, InNumConstantBuffers, InNumShaderResourceViews, InNumSamplers, InNumUnorderedAccessViews)
	{
	}
};

class D3D12PixelShader : public IPixelShader, public D3D12Shader<SHADER_STAGE_PIXEL>
{
public:
	D3D12PixelShader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews)
		: D3D12Shader(InByteCode, InCompiledCode, InNumConstantBuffers, InNumShaderResourceViews, InNumSamplers, InNumUnorderedAccessViews)
	{
	}
};

class D3D12HullShader : public IHullShader, public D3D12Shader<SHADER_STAGE_HULL>
{
public:
	D3D12HullShader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews)
		: D3D12Shader(InByteCode, InCompiledCode, InNumConstantBuffers, InNumShaderResourceViews, InNumSamplers, InNumUnorderedAccessViews)
	{
	}
};

class D3D12GeometryShader : public IGeometryShader, public D3D12Shader<SHADER_STAGE_GEOMETRY>
{
public:
	D3D12GeometryShader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews)
		: D3D12Shader(InByteCode, InCompiledCode, InNumConstantBuffers, InNumShaderResourceViews, InNumSamplers, InNumUnorderedAccessViews)
	{
	}
};

class D3D12ComputeShader : public IComputeShader, public D3D12Shader<SHADER_STAGE_COMPUTE>
{
public:
	D3D12ComputeShader(const std::string& InByteCode, const std::vector<uint8>& InCompiledCode, uint InNumConstantBuffers, uint InNumShaderResourceViews, uint InNumSamplers, uint InNumUnorderedAccessViews)
		: D3D12Shader(InByteCode, InCompiledCode, InNumConstantBuffers, InNumShaderResourceViews, InNumSamplers, InNumUnorderedAccessViews)
	{
	}
};

class D3D12InputLayout : public IInputLayout
{
public:
	D3D12InputLayout(GFX_INPUT_LAYOUT_DESC InDesc, D3D12_INPUT_LAYOUT_DESC InInputDesc) :
		Desc(InDesc),
		InputDesc(InInputDesc)
	{
	}

	~D3D12InputLayout()
	{
		delete InputDesc.pInputElementDescs;
	}

	GFX_INPUT_LAYOUT_DESC GetDesc() const final override 
	{
		return Desc;
	}

	inline uint GetStride() const
	{
		return GetLayoutDescSize(Desc);
	}

	GFX_INPUT_LAYOUT_DESC Desc;
	D3D12_INPUT_LAYOUT_DESC InputDesc;
};

class D3D12DepthStencilState : public IDepthStencilState
{
public:

	D3D12DepthStencilState(const GFX_DEPTH_STENCIL_DESC& InDesc) :
		Desc(InDesc)
	{
	}

	virtual const GFX_DEPTH_STENCIL_DESC GetDesc() const final override
	{
		return Desc;
	}

	GFX_DEPTH_STENCIL_DESC Desc;
};

class D3D12GraphicsPipelineState : public IGraphicsPipelineState
{
public:
	D3D12GraphicsPipelineState();
	D3D12GraphicsPipelineState(ID3D12Device1* Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, const GFX_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);
	D3D12GraphicsPipelineState(ID3D12PipelineState* InPipelineState, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, const GFX_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);
	~D3D12GraphicsPipelineState();

	virtual void GetDesc(GFX_PIPELINE_STATE_DESC* OutDesc) const final override;
	virtual uint8* GetSerialized(uint& OutBytes) const final override;

	void Reset(ID3D12Device1* Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, const GFX_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);

	GFX_PIPELINE_STATE_DESC				Desc;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	CreationDesc;
	ID3D12PipelineState*				PipelineState;
	D3D12ShaderSignature				ShaderSignature;
};

class D3D12ComputePipelineState : public IComputePipelineState
{
public:
	D3D12ComputePipelineState();
	D3D12ComputePipelineState(ID3D12Device1* Device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, const COMPUTE_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);
	D3D12ComputePipelineState(ID3D12PipelineState* InPipeline, const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, const COMPUTE_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);
	~D3D12ComputePipelineState();

	virtual void GetDesc(COMPUTE_PIPELINE_STATE_DESC* OutDesc) const final override;
	virtual uint8* GetSerialized(uint& OutBytes) const final override;

	void Reset(ID3D12Device1* Device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, const COMPUTE_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature);

	COMPUTE_PIPELINE_STATE_DESC			Desc;
	D3D12_COMPUTE_PIPELINE_STATE_DESC	CreationDesc;
	ID3D12PipelineState*				PipelineState;
	D3D12ShaderSignature				ShaderSignature;
};
