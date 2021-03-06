#include "D3D12PipelineState.h"
#include <Math/SIMDFunctions.h>


D3D12GraphicsPipelineState::D3D12GraphicsPipelineState() :
	CreationDesc(),
	PipelineState(nullptr),
	ShaderSignature()
{
}

D3D12GraphicsPipelineState::D3D12GraphicsPipelineState(ID3D12Device1* Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, const GFX_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature) :
	PipelineState(nullptr)
{
	Reset(Device, InCreationDesc, InDesc, InShaderSignature);
}

D3D12GraphicsPipelineState::D3D12GraphicsPipelineState(ID3D12PipelineState* InPipelineState, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, const GFX_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature) :
	PipelineState(InPipelineState),
	CreationDesc(InCreationDesc),
	ShaderSignature(InShaderSignature)
{
	Desc = InDesc;
}

D3D12GraphicsPipelineState::~D3D12GraphicsPipelineState()
{
	PipelineState->Release();
}

uint8* D3D12GraphicsPipelineState::GetSerialized(uint32& OutBytes) const
{
	ID3DBlob* WrittenBlob;
	PipelineState->GetCachedBlob(&WrittenBlob);

	uint8* BytesToWrite = new uint8[WrittenBlob->GetBufferSize() + 2];
	BytesToWrite[0] = '\n';
	BytesToWrite[WrittenBlob->GetBufferSize()] = '\n';
	SIMDMemcpy(&BytesToWrite[1], WrittenBlob->GetBufferPointer(), static_cast<uint32>(WrittenBlob->GetBufferSize()));

	OutBytes = static_cast<uint32>(WrittenBlob->GetBufferSize());
	return BytesToWrite;
}

void D3D12GraphicsPipelineState::Reset(
	ID3D12Device1* Device, 
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& InCreationDesc, 
	const GFX_PIPELINE_STATE_DESC& InDesc, 
	D3D12ShaderSignature InShaderSignature)
{
	if (PipelineState != nullptr)
	{
		PipelineState->Release();
		PipelineState = nullptr;
	}
	Desc = InDesc;
	CreationDesc = InCreationDesc;
	ShaderSignature = InShaderSignature;
	D3D12ERRORCHECK(Device->CreateGraphicsPipelineState(&CreationDesc, IID_PPV_ARGS(&PipelineState)));
}

D3D12ComputePipelineState::D3D12ComputePipelineState() :
	CreationDesc(),
	PipelineState(nullptr),
	ShaderSignature()
{
}

D3D12ComputePipelineState::D3D12ComputePipelineState(ID3D12Device1* Device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, const COMPUTE_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature) :
	PipelineState(nullptr)
{
	Reset(Device, InCreationDesc, InDesc, InShaderSignature);
}

D3D12ComputePipelineState::D3D12ComputePipelineState(ID3D12PipelineState* InPipeline, const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, const COMPUTE_PIPELINE_STATE_DESC& InDesc, D3D12ShaderSignature InShaderSignature) :
	PipelineState(InPipeline),
	CreationDesc(InCreationDesc),
	ShaderSignature(InShaderSignature)
{
	Desc = InDesc;
}

D3D12ComputePipelineState::~D3D12ComputePipelineState()
{
	PipelineState->Release();
}

uint8* D3D12ComputePipelineState::GetSerialized(uint32& OutBytes) const
{
	UNUSED(OutBytes);
	return nullptr;
}

void D3D12ComputePipelineState::Reset(
	ID3D12Device1* Device, 
	const D3D12_COMPUTE_PIPELINE_STATE_DESC& InCreationDesc, 
	const COMPUTE_PIPELINE_STATE_DESC& InDesc, 
	D3D12ShaderSignature InShaderSignature)
{
	if (PipelineState != nullptr)
	{
		PipelineState->Release();
		PipelineState = nullptr;
	}
	CreationDesc = InCreationDesc;
	Desc = InDesc;
	ShaderSignature = InShaderSignature;
	D3D12ERRORCHECK(Device->CreateComputePipelineState(&CreationDesc, IID_PPV_ARGS(&PipelineState)));
}
