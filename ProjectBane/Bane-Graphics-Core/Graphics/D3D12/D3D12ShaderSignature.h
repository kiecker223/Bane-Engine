#pragma once

#include "Common.h"
#include "../Interfaces/PipelineState.h"
#include <string>
#include <Core/Containers/Array.h>
#include <d3d12.h>
#include "D3D12Helper.h"

typedef struct D3D12_DESCRIPTOR_PARAMETER {
	D3D12_DESCRIPTOR_RANGE_TYPE Type;
	uint32						Slot;
} D3D12_DESCRIPTOR_PARAMETER;

typedef enum ED3D12_SHADER_PARAMETER_TYPE {
	D3D12_SHADER_PARAMETER_TYPE_CBV,
	D3D12_SHADER_PARAMETER_TYPE_SRV,
	D3D12_SHADER_PARAMETER_TYPE_UAV,
	D3D12_SHADER_PARAMETER_NUM_TYPES
} ED3D12_SHADER_PARAMETER_TYPE;


typedef struct D3D12_SHADER_PARAMETER {
	D3D12_SHADER_PARAMETER()  { }
	~D3D12_SHADER_PARAMETER() { }

	ED3D12_SHADER_PARAMETER_TYPE Type;
	uint32 ShaderRegister;

} D3D12_SHADER_PARAMETER;

inline bool operator == (const D3D12_SHADER_PARAMETER& Left, const D3D12_SHADER_PARAMETER& Right)
{
	return (Left.Type == Right.Type && Left.ShaderRegister == Right.ShaderRegister);
}

inline bool operator != (const D3D12_SHADER_PARAMETER& Left, const D3D12_SHADER_PARAMETER& Right)
{
	return !(Left == Right);
}

class D3D12ShaderSignatureParameterList 
{
public:

	inline void AddParameter(const D3D12_SHADER_PARAMETER& Parameter)
	{
		ShaderParams.Add(Parameter);
		RootParamIndexes[(uint32)Parameter.Type].Add(NumParameters);
		NumParameters++;
		switch (Parameter.Type)
		{
		case D3D12_SHADER_PARAMETER_TYPE_CBV:
			NumConstantBuffers++;
			break;
		case D3D12_SHADER_PARAMETER_TYPE_SRV:
			NumShaderResourceViews++;
			break;
		case D3D12_SHADER_PARAMETER_TYPE_UAV:
			NumUnorderedAccessViews++;
			break;
		}
	}

	inline void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& SamplerDesc)
	{
		StaticSamplers.Add(SamplerDesc);
	}
	
	uint32 NumConstantBuffers;
	uint32 NumShaderResourceViews;
	uint32 NumUnorderedAccessViews;
	TArray<D3D12_SHADER_PARAMETER> ShaderParams;
	TArray<uint32> RootParamIndexes[D3D12_SHADER_PARAMETER_NUM_TYPES];
	uint32 NumParameters;
	TArray<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;
};

inline bool operator == (const D3D12ShaderSignatureParameterList& Left, const D3D12ShaderSignatureParameterList& Right)
{
	if (Left.NumConstantBuffers != Right.NumConstantBuffers)
	{
		return false;
	}
	if (Left.NumShaderResourceViews != Right.NumShaderResourceViews)
	{
		return false;
	}
	if (Left.NumUnorderedAccessViews != Right.NumUnorderedAccessViews)
	{
		return false;
	}
	if (Left.ShaderParams.GetCount() == Right.ShaderParams.GetCount())
	{
		for (uint32 i = 0; i < Left.ShaderParams.GetCount(); i++)
		{
			if (Left.ShaderParams[i] != Right.ShaderParams[i])
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	if (Left.StaticSamplers.GetCount() == Right.StaticSamplers.GetCount())
	{
		for (uint32 i = 0; i < Left.StaticSamplers.GetCount(); i++)
		{
			if (Left.StaticSamplers[i] != Right.StaticSamplers[i])
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}



class D3D12ShaderItemData
{
public:

	D3D12ShaderItemData() : NumCBVs(0), NumSRVs(0), NumUAVs(0), NumSMPs(0) { }
	D3D12ShaderItemData(const PIPELINE_STATE_RESOURCE_COUNTS& Counts) :
		NumCBVs(static_cast<uint32>(Counts.NumConstantBuffers)),
		NumSRVs(static_cast<uint32>(Counts.NumShaderResourceViews)),
		NumUAVs(static_cast<uint32>(Counts.NumUnorderedAccessViews)),
		NumSMPs(static_cast<uint32>(Counts.NumSamplers))
	{ }
	D3D12ShaderItemData(uint32 InNumCBVs, uint32 InNumSRVs, uint32 InNumUAVs, uint32 InNumSMPs) : NumCBVs(InNumCBVs), NumSRVs(InNumSRVs), NumUAVs(InNumUAVs), NumSMPs(InNumSMPs) { }

	inline ESHADER_PARAMETER_TYPE GetLargestParameterType() const
	{
		if (NumCBVs > NumSRVs && NumCBVs > NumUAVs)
		{
			return SHADER_PARAMETER_TYPE_CBV;
		}
		if (NumSRVs > NumCBVs && NumSRVs > NumUAVs)
		{
			return SHADER_PARAMETER_TYPE_SRV;
		}
		if (NumUAVs > NumCBVs && NumUAVs > NumSRVs)
		{
			return SHADER_PARAMETER_TYPE_UAV;
		}
		return SHADER_PARAMETER_TYPE_CBV; // Here its safe to assume that they are all equal
	}

	inline uint32 GetLargestParameterSize() const
	{
		ESHADER_PARAMETER_TYPE LargestParameterType = GetLargestParameterType();
		switch (LargestParameterType)
		{
		case SHADER_PARAMETER_TYPE_CBV:
			return NumCBVs;
		case SHADER_PARAMETER_TYPE_SRV:
			return NumSRVs;
		case SHADER_PARAMETER_TYPE_UAV:
			return NumUAVs;
		}
		return 0;
	}

	PIPELINE_STATE_RESOURCE_COUNTS ToResourceCounts()
	{
		return { static_cast<uint8>(NumCBVs), static_cast<uint8>(NumSRVs), static_cast<uint8>(NumSMPs), static_cast<uint8>(NumUAVs) };
	}

	operator PIPELINE_STATE_RESOURCE_COUNTS ()
	{
		return ToResourceCounts();
	}

	inline uint32 TotalParameterCount() const
	{
		uint32 Result = 0;
		Result += NumCBVs;
		Result += NumSRVs;
		Result += NumUAVs;
		return Result;
	}

	void Quantize();
	inline D3D12ShaderItemData Quantized()
	{
		D3D12ShaderItemData Result(*this);
		Result.Quantize();
		return Result;
	}

	uint32 NumCBVs;
	uint32 NumSRVs;
	uint32 NumUAVs;
	uint32 NumSMPs;
};


class D3D12ShaderSignature
{
public:

	D3D12ShaderSignature() : RootSignature(nullptr) { }

	inline void Delete()
	{
		if (RootSignature)
		{
			RootSignature->Release();
		}
	}

	inline uint32 GetCBVIndex(uint32 Register) const
	{
		return Register;
	}

	inline uint32 GetSRVTableIndex() const
	{
		return ShaderData.NumCBVs;
	}

	inline uint32 GetSamplerTableIndex() const
	{
		return GetSRVTableIndex() + 1;
	}

	inline uint32 GetUAVTableIndex() const
	{
		BANE_CHECK(HasUAVs());
		return GetSamplerTableIndex() + 1;
	}

	inline bool HasUAVs() const
	{
		return ShaderData.NumUAVs > 0;
	}

	D3D12ShaderItemData ShaderData;
	ID3D12RootSignature* RootSignature;
};
