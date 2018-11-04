#include "D3D12ShaderResourceViews.h"
#include "D3D12ShaderSignatureLibrary.h"
#include "D3D12GraphicsDevice.h"



D3D12DepthStencilView::FRAME_DATA& D3D12DepthStencilView::GetCurrentFrame()
{
	return FrameData[D3D12GraphicsDevice::GetCurrentFrameIndex()];
}

D3D12RenderTargetView::FRAME_DATA& D3D12RenderTargetView::GetCurrentFrame()
{
	return FrameData[D3D12GraphicsDevice::GetCurrentFrameIndex()];
}


D3D12_CPU_DESCRIPTOR_HANDLE* D3D12RenderPassInfo::GetDSV()
{
	if (DepthStencil)
	{
		return &DepthStencil->FrameData[D3D12GraphicsDevice::GetCurrentFrameIndex()].CpuHandle;
	}
	return nullptr;
}

D3D12ShaderResourceTable::D3D12ShaderResourceTable(
	uint32 InNumCBVs, 
	uint32 InNumSRVs, 
	uint32 InNumSMPs, 
	uint32 InNumUAVs, 
	D3D12DescriptorAllocation InSRVAllocation, 
	D3D12DescriptorAllocation InSMPAllocation, 
	D3D12DescriptorAllocation InUAVAllocation) :
		NumCBVs(InNumCBVs),
		NumSMPs(InNumSMPs),
		NumSRVs(InNumSRVs),
		NumUAVs(InNumUAVs),
		BaseSRVAllocation(InSRVAllocation),
		BaseSMPAllocation(InSMPAllocation),
		BaseUAVAllocation(InUAVAllocation)
{
	D3D12ShaderItemData ShaderData(NumCBVs, NumSRVs, NumUAVs, NumSMPs);
	AssociatedSignature = GetD3D12ShaderSignatureLibrary()->GetSignature(ShaderData);
	ConstantBuffers.Resize(InNumCBVs, { nullptr, 0 });
	ShaderResources.Resize(InNumSRVs, nullptr);
	Samplers.Resize(InNumSMPs, nullptr);
	UnorderedAccessResources.Resize(InNumUAVs, nullptr);
}