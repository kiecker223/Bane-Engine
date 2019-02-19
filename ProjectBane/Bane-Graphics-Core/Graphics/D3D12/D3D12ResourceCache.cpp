#include "D3D12ResourceCache.h"
#include "D3D12CommandContext.h"
#include "D3D12GraphicsDevice.h"

void D3D12ResourceCache::Initialize(D3D12GraphicsPipelineState* pPipeline, D3D12GraphicsDevice* pDevice)
{
	Device = pDevice->GetDevice();
	Counts = pPipeline->Desc.Counts;
	SrvAllocation = pDevice->GetSrvAllocator().AllocateMultiple(Counts.NumShaderResourceViews);
	SmpAllocation = pDevice->GetSmpAllocator().AllocateMultiple(Counts.NumSamplers);
	if (Counts.NumUnorderedAccessViews > 0)
	{
		UavAllocation = pDevice->GetSrvAllocator().AllocateMultiple(Counts.NumUnorderedAccessViews);
	}
}

void D3D12ResourceCache::Initialize(D3D12ComputePipelineState* pPipeline, D3D12GraphicsDevice* pDevice)
{
	Device = pDevice->GetDevice();
	Counts = pPipeline->Desc.Counts;
	SrvAllocation = pDevice->GetSrvAllocator().AllocateMultiple(Counts.NumShaderResourceViews);
	SmpAllocation = pDevice->GetSmpAllocator().AllocateMultiple(Counts.NumSamplers);
	if (Counts.NumUnorderedAccessViews > 0)
	{
		UavAllocation = pDevice->GetSrvAllocator().AllocateMultiple(Counts.NumUnorderedAccessViews);
	}
}

void D3D12ResourceCache::ApplyGraphicsResources(D3D12GraphicsCommandBuffer* pCmdList)
{
	ID3D12GraphicsCommandList* pCL = pCmdList->D3DCL;
	D3D12ShaderSignature RootSignature = pCmdList->RootSignature;

	bAnySrvDirty = false;
	bAnyCbvDirty = false;
	bAnyUavDirty = false;
	for (uint32 i = 0; i < Counts.NumShaderResourceViews; i++)
	{
		if (DirtySrvs[i])
		{
			D3D12_SHADER_RESOURCE Srv = Srvs[i];
			Device->CreateShaderResourceView(Srv.pResource, &Srv.Desc, SrvAllocation.OffsetFromStart(i).CpuHandle);
			if (Srv.Desc.ViewDimension != D3D12_SRV_DIMENSION_BUFFER)
			{
				Device->CreateSampler(&Srv.Sampler, SmpAllocation.OffsetFromStart(i).CpuHandle);
			}
		}
	}
	pCL->SetGraphicsRootDescriptorTable(RootSignature.GetSRVTableIndex(), SrvAllocation.GpuHandle);
	pCL->SetGraphicsRootDescriptorTable(RootSignature.GetSamplerTableIndex(), SmpAllocation.GpuHandle);
	for (uint32 i = 0; i < Counts.NumConstantBuffers; i++)
	{
		if (DirtyCbvs[i])
		{
			D3D12_CONSTANT_BUFFER Cbv = Cbvs[i];
			pCL->SetGraphicsRootConstantBufferView(i, Cbv.Address);
		}
	}
	if (Counts.NumUnorderedAccessViews)
	{
		for (uint32 i = 0; i < Counts.NumShaderResourceViews; i++)
		{
			if (DirtyUavs[i])
			{
				D3D12_UNORDERED_ACCESS_RESOURCE Uav = Uavs[i];
				Device->CreateUnorderedAccessView(Uav.pResource, nullptr, &Uav.Desc, UavAllocation.OffsetFromStart(i).CpuHandle);
			}
		}
		pCL->SetGraphicsRootDescriptorTable(RootSignature.GetUAVTableIndex(), UavAllocation.GpuHandle);
	}
	memset(DirtySrvs, 0, sizeof(DirtySrvs));
	memset(DirtyCbvs, 0, sizeof(DirtyCbvs));
	memset(DirtyUavs, 0, sizeof(DirtyUavs));
}

void D3D12ResourceCache::ApplyComputeResources(D3D12GraphicsCommandBuffer* pCmdList)
{
	ID3D12GraphicsCommandList* pCL = pCmdList->D3DCL;
	D3D12ShaderSignature RootSignature = pCmdList->RootSignature;
	
	bAnySrvDirty = false;
	bAnyCbvDirty = false;
	bAnyUavDirty = false;

	for (uint32 i = 0; i < Counts.NumShaderResourceViews; i++)
	{
		if (DirtySrvs[i])
		{
			D3D12_SHADER_RESOURCE Srv = Srvs[i];
			Device->CreateShaderResourceView(Srv.pResource, &Srv.Desc, SrvAllocation.OffsetFromStart(i).CpuHandle);
			if (Srv.Desc.ViewDimension != D3D12_SRV_DIMENSION_BUFFER)
			{
				Device->CreateSampler(&Srv.Sampler, SmpAllocation.OffsetFromStart(i).CpuHandle);
			}
		}
	}
	pCL->SetComputeRootDescriptorTable(RootSignature.GetSRVTableIndex(), SrvAllocation.GpuHandle);
	pCL->SetComputeRootDescriptorTable(RootSignature.GetSamplerTableIndex(), SmpAllocation.GpuHandle);
	for (uint32 i = 0; i < Counts.NumConstantBuffers; i++)
	{
		if (DirtyCbvs[i])
		{
			D3D12_CONSTANT_BUFFER Cbv = Cbvs[i];
			pCL->SetComputeRootConstantBufferView(i, Cbv.Address);
		}
	}
	if (Counts.NumUnorderedAccessViews)
	{
		for (uint32 i = 0; i < Counts.NumShaderResourceViews; i++)
		{
			if (DirtyUavs[i])
			{
				D3D12_UNORDERED_ACCESS_RESOURCE Uav = Uavs[i];
				Device->CreateUnorderedAccessView(Uav.pResource, nullptr, &Uav.Desc, UavAllocation.OffsetFromStart(i).CpuHandle);
			}
		}
		pCL->SetComputeRootDescriptorTable(RootSignature.GetUAVTableIndex(), UavAllocation.GpuHandle);
	}
	memset(DirtySrvs, 0, sizeof(DirtySrvs));
	memset(DirtyCbvs, 0, sizeof(DirtyCbvs));
	memset(DirtyUavs, 0, sizeof(DirtyUavs));
}
