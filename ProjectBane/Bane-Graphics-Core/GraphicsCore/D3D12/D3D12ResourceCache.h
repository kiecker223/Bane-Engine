#pragma once
#include "D3D12DescriptorAllocator.h"
#include "D3D12GraphicsResource.h"
#include "D3D12PipelineState.h"

class D3D12GraphicsCommandBuffer;
class D3D12GraphicsDevice;

// Fills out a parameter list for each shader
// This is PER DRAW CALL
class D3D12ResourceCache
{
public:

	D3D12ResourceCache()
	{
		memset(DirtySrvs, 0, sizeof(DirtySrvs));
		memset(DirtyCbvs, 0, sizeof(DirtyCbvs));
		memset(DirtyUavs, 0, sizeof(DirtyUavs));
	}

	struct D3D12_SHADER_RESOURCE
	{
		ID3D12Resource* pResource;
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
		D3D12_SAMPLER_DESC Sampler; // optional
	};

	struct D3D12_UNORDERED_ACCESS_RESOURCE
	{
		ID3D12Resource* pResource;
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
	};

	struct D3D12_CONSTANT_BUFFER
	{
		D3D12_GPU_VIRTUAL_ADDRESS Address;
	};

	bool DirtySrvs[16];
	bool bAnySrvDirty;
	bool DirtyUavs[16];
	bool bAnyUavDirty;
	bool DirtyCbvs[16];
	bool bAnyCbvDirty;

	D3D12_SHADER_RESOURCE				Srvs[16];
	D3D12_UNORDERED_ACCESS_RESOURCE		Uavs[16];
	D3D12_CONSTANT_BUFFER				Cbvs[16];
	PIPELINE_STATE_RESOURCE_COUNTS		Counts;

	D3D12DescriptorAllocation SrvAllocation;
	D3D12DescriptorAllocation SmpAllocation;
	D3D12DescriptorAllocation UavAllocation;

	void Initialize(D3D12GraphicsPipelineState* pPipeline, D3D12GraphicsDevice* pDevice, D3D12LinearDescriptorAllocator& SrvAllocator, D3D12LinearDescriptorAllocator& SmpAllocator);
	void Initialize(D3D12ComputePipelineState* pPipeline, D3D12GraphicsDevice* pDevice, D3D12LinearDescriptorAllocator& SrvAllocator, D3D12LinearDescriptorAllocator& SmpAllocator);

	inline void SetSRV(const D3D12TextureBase* Texture, uint32 Slot, uint32 Subresource)
	{
		BANE_CHECK(Slot < 16);
		if (Texture)
		{
			Srvs[Slot] = { Texture->Resource.D3DResource, Texture->GetSRVDesc(Subresource), Texture->D3DSampleDesc };
			DirtySrvs[Slot] = true;
			bAnySrvDirty = true;
		}
	}

	inline void SetSRV(const D3D12Buffer* StructuredBuffer, uint32 Slot, uint64 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
	{
		BANE_CHECK(Slot < 16);
		if (StructuredBuffer)
		{
			Srvs[Slot] = { StructuredBuffer->Resource.D3DResource, StructuredBuffer->GetSRVDesc(IndexToStart, NumElements, StructureByteStride), {} };
			DirtySrvs[Slot] = true;
			bAnySrvDirty = true;
		}
	}

	inline void SetCBV(const D3D12Buffer* ConstantBuffer, uint64 Offset, uint32 Slot)
	{
		BANE_CHECK(Slot < 16);
		if (ConstantBuffer)
		{
			Cbvs[Slot] = { ConstantBuffer->Resource.Location + Offset };
			DirtyCbvs[Slot] = true;
			bAnyCbvDirty = true;
		}
	}

	inline void SetUAV(const D3D12TextureBase* Texture, uint32 Subresource, uint32 Slot)
	{
		BANE_CHECK(Slot < 16);
		if (Texture)
		{
			Uavs[Slot] = { Texture->Resource.D3DResource, Texture->GetUAVDesc(Subresource) };
			DirtyUavs[Slot] = true;
			bAnyUavDirty = true;
		}
	}

	inline void SetUAV(const D3D12Buffer* Buffer, uint32 Slot, uint64 IndexToStart, uint32 NumElements, uint32 StructureByteStride)
	{
		BANE_CHECK(Slot < 16);
		if (Buffer)
		{
			Uavs[Slot] = { Buffer->Resource.D3DResource, Buffer->GetUAVDesc(IndexToStart, NumElements, StructureByteStride) };
			DirtyUavs[Slot] = true;
			bAnyUavDirty = true;
		}
	}

	void ApplyGraphicsResources(D3D12GraphicsCommandBuffer* pCmdList);
	void ApplyComputeResources(D3D12GraphicsCommandBuffer* pCmdList);

	ID3D12Device* Device;
};
