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
