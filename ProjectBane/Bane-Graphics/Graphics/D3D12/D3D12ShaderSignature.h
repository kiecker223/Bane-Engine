#pragma once

#include "Core/Common.h"
#include "../Interfaces/PipelineState.h"
#include <string>
#include <vector>
#include <d3d12.h>
#include "D3D12Helper.h"

typedef struct D3D12_DESCRIPTOR_PARAMETER {
	D3D12_DESCRIPTOR_RANGE_TYPE Type;
	uint						Slot;
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
	uint ShaderRegister;

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
		ShaderParams.push_back(Parameter);
		RootParamIndexes[(uint)Parameter.Type].push_back(NumParameters);
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
		StaticSamplers.push_back(SamplerDesc);
	}
	
	uint NumConstantBuffers;
	uint NumShaderResourceViews;
	uint NumUnorderedAccessViews;
	std::vector<D3D12_SHADER_PARAMETER> ShaderParams;
	std::vector<uint> RootParamIndexes[D3D12_SHADER_PARAMETER_NUM_TYPES];
	uint NumParameters;
	std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers;
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
	if (Left.ShaderParams.size() == Right.ShaderParams.size())
	{
		for (uint i = 0; i < Left.ShaderParams.size(); i++)
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
	if (Left.StaticSamplers.size() == Right.StaticSamplers.size())
	{
		for (uint i = 0; i < Left.StaticSamplers.size(); i++)
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
	D3D12ShaderItemData(uint InNumCBVs, uint InNumSRVs, uint InNumUAVs, uint InNumSMPs) : NumCBVs(InNumCBVs), NumSRVs(InNumSRVs), NumUAVs(InNumUAVs), NumSMPs(InNumSMPs) { }

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

	inline uint GetLargestParameterSize() const
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

	inline uint TotalParameterCount() const
	{
		uint Result = 0;
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

	uint NumCBVs;
	uint NumSRVs;
	uint NumUAVs;
	uint NumSMPs;
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

	inline uint GetCBVIndex(uint Register) const
	{
		return Register;
	}

	inline uint GetSRVTableIndex() const
	{
		return ShaderData.NumCBVs;
	}

	inline uint GetSamplerTableIndex() const
	{
		return GetSRVTableIndex() + 1;
	}

	inline uint GetUAVTableIndex() const
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
