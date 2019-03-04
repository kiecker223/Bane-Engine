#pragma once

#include "../Interfaces/ShaderResourceView.h"
#include "D3D12DescriptorAllocator.h"
#include "D3D12GraphicsResource.h"
#include <d3d12.h>

class D3D12GraphicsCommandContext;


class D3D12DepthStencilView : public IDepthStencilView
{
public:
	struct FRAME_DATA
	{
		D3D12TextureBase* Texture;
		D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
	} FrameData[3]; // Three depth stenils are always allocated. Avoid race conditions

	FRAME_DATA& GetCurrentFrame();

};

class D3D12RenderTargetView : public IRenderTargetView
{
public:

	struct FRAME_DATA
	{
		D3D12TextureBase* Texture;
		D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle;
	} FrameData[3]; // Three render targets are always allocated, Avoid race conditions

	FRAME_DATA& GetCurrentFrame();
};

class D3D12RenderPassInfo : public IRenderTargetInfo
{
public:

	D3D12RenderPassInfo(const IRenderTargetView** Rtvs, uint32 InNumRenderTargets, const IDepthStencilView* Dsv, const fvec4& InClearColor) :
		NumRenderTargets(InNumRenderTargets),
		DepthStencil((D3D12DepthStencilView*)Dsv),
		ClearColor(InClearColor)
	{
		for (uint32 i = 0; i < NumRenderTargets; i++)
		{
			RenderTargets[i] = (D3D12RenderTargetView*)Rtvs[i];
		}
	}

	IRenderTargetView* GetRenderTarget(uint32 Index) const final override
	{
		return RenderTargets[Index];
	}

	IRenderTargetView** GetRenderTargets() const final override
	{
		return (IRenderTargetView**)&RenderTargets[0];
	}

	uint32 GetNumRenderTargets() const final override
	{
		return NumRenderTargets;
	}

	IDepthStencilView* GetDepthStencil() const final override
	{
		return DepthStencil;
	}

	fvec4 GetClearColor() const final override
	{
		return ClearColor;
	}

	inline bool HasDepthStencil() const
	{
		return DepthStencil != nullptr;
	}

	inline void SetRenderTargets(ID3D12GraphicsCommandList* CommandList)
	{
		{
			D3D12_CPU_DESCRIPTOR_HANDLE Rtvs[16] = { { } };
			D3D12_CPU_DESCRIPTOR_HANDLE* Dsv = nullptr;
			for (uint32 i = 0; i < NumRenderTargets; i++)
			{
				Rtvs[i] = RenderTargets[i]->GetCurrentFrame().CpuHandle;
			}
			if (HasDepthStencil())
			{
				Dsv = &DepthStencil->GetCurrentFrame().CpuHandle;
			}

			CommandList->OMSetRenderTargets(NumRenderTargets, Rtvs, FALSE, Dsv);
		}
	}

	inline void Clear(ID3D12GraphicsCommandList* CommandList)
	{
		for (uint32 i = 0; i < NumRenderTargets; i++)
		{
			D3D12_CPU_DESCRIPTOR_HANDLE Rtv = RenderTargets[i]->GetCurrentFrame().CpuHandle;
			CommandList->ClearRenderTargetView(Rtv, &ClearColor.x, 0, nullptr);
		}
		if (HasDepthStencil())
		{
			CommandList->ClearDepthStencilView(DepthStencil->GetCurrentFrame().CpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		}
	}

	inline void TransitionResourcesToWrite(D3D12GraphicsCommandBuffer* Context)
	{
		for (uint32 i = 0; i < NumRenderTargets; i++)
		{
			auto* Tex = RenderTargets[i]->GetCurrentFrame().Texture;
			Tex->TransitionResource(Context, D3D12_RESOURCE_STATE_RENDER_TARGET);
			Tex->PromotedState = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		if (HasDepthStencil())
		{
			auto* Tex = DepthStencil->GetCurrentFrame().Texture;
			Tex->TransitionResource(Context, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			Tex->PromotedState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}
	}

	inline void TransitionResourcesToRead(D3D12GraphicsCommandBuffer* Context)
	{
		for (uint32 i = 0; i < NumRenderTargets; i++)
		{
			auto* Tex = RenderTargets[i]->GetCurrentFrame().Texture;
			Tex->TransitionResource(Context, D3D12_RESOURCE_STATE_PRESENT);
			Tex->PromotedState = D3D12_RESOURCE_STATE_PRESENT;
		}
		if (HasDepthStencil())
		{
			auto* Tex = DepthStencil->GetCurrentFrame().Texture;
			Tex->TransitionResource(Context, D3D12_RESOURCE_STATE_DEPTH_READ);
			Tex->PromotedState = D3D12_RESOURCE_STATE_DEPTH_READ;
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE* GetDSV();

	D3D12RenderTargetView* RenderTargets[16];
	uint32 NumRenderTargets;
	D3D12DepthStencilView* DepthStencil;
	const fvec4 ClearColor;
};
