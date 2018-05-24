#pragma once
#include <map>
#include "D3D12ShaderSignature.h"

#define D3D12_MAX_SHADER_SIGNATURES 14

typedef enum ED3D12_SHADER_SIGNATURE_TIER {
	D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16SAMPLER,
//	D3D12_SHADER_SIGNATURE_TIER_2UAV,
//	D3D12_SHADER_SIGNATURE_TIER_4UAV,
//	D3D12_SHADER_SIGNATURE_TIER_2CBV_2UAV,
//	D3D12_SHADER_SIGNATURE_TIER_4CBV_4UAV,
	D3D12_SHADER_SIGNATURE_TIER_1CBV_1SRV_1UAV_1SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_2CBV_2SRV_2UAV_2SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_4CBV_4SRV_4UAV_4SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_8CBV_8SRV_8UAV_8SAMPLER,
	D3D12_SHADER_SIGNATURE_TIER_16CBV_16SRV_16UAV_16SAMPLER,
	D3D12_SHADER_SIGNATURE_NUM_TIERS
} ED3D12_SHADER_SIGNATURE_TIER;

ForceInline void InitializeD3D12ShaderSignatureLibrary(ID3D12Device1* Device);
ForceInline void DestroyD3D12ShaderSignatureLibrary();


class D3D12ShaderSignatureLibrary
{

public:

	void Initialize(ID3D12Device1* Device);
	void Shutdown();
	D3D12ShaderSignature GetSignature(const D3D12ShaderItemData& ParameterList);

	D3D12ShaderSignature DetermineBestRootSignature(
		class D3D12VertexShader* VertexShader, 
		class D3D12PixelShader* PixelShader, 
		class D3D12HullShader* HullShader = nullptr, 
		class D3D12GeometryShader* GeometryShader = nullptr);
	D3D12ShaderSignature DetermineBestComputeSignature(class D3D12ComputeShader* ComputeShader);

	static D3D12ShaderSignatureLibrary* Get()
	{
		return GD3D12ShaderSignatureLibraryInstance;
	}

private:

	friend void InitializeD3D12ShaderSignatureLibrary(ID3D12Device1* Device);
	friend void DestroyD3D12ShaderSignatureLibrary();

	ED3D12_SHADER_SIGNATURE_TIER GetTier(const D3D12ShaderItemData& Data);

	static D3D12ShaderSignatureLibrary* GD3D12ShaderSignatureLibraryInstance;

	D3D12ShaderSignature m_TieredSignatures[D3D12_MAX_SHADER_SIGNATURES];
};

ForceInline void InitializeD3D12ShaderSignatureLibrary(ID3D12Device1* Device)
{
	D3D12ShaderSignatureLibrary::GD3D12ShaderSignatureLibraryInstance = new D3D12ShaderSignatureLibrary();
	D3D12ShaderSignatureLibrary::Get()->Initialize(Device);
}

ForceInline void DestroyD3D12ShaderSignatureLibrary()
{
	D3D12ShaderSignatureLibrary::Get()->Shutdown();
	delete D3D12ShaderSignatureLibrary::GD3D12ShaderSignatureLibraryInstance;
}

ForceInline D3D12ShaderSignatureLibrary* GetD3D12ShaderSignatureLibrary()
{
	return D3D12ShaderSignatureLibrary::Get();
}