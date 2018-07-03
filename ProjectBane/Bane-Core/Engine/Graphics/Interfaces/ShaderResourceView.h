#pragma once
#include "Core/KieckerMath.h"

class IGPUResource;



class IShaderResourceTable
{
public:
	virtual ~IShaderResourceTable() { }
	
	virtual uint GetNumConstantBuffers() const = 0;
	virtual uint GetNumTextures() const = 0;
	virtual uint GetNumSamplers() const = 0;
	virtual uint GetNumUnorderedAccessViews() const = 0;

};

class IRenderTargetView
{
public:
	virtual ~IRenderTargetView() { }
};

class IDepthStencilView
{
public:
	virtual ~IDepthStencilView() { }
};

class IRenderPassInfo
{
public:
	virtual ~IRenderPassInfo() { }

	virtual IRenderTargetView** GetRenderTargets() const = 0;
	virtual uint GetNumRenderTargets() const = 0;
	virtual IRenderTargetView* GetRenderTarget(uint Index) const = 0;
	virtual IDepthStencilView* GetDepthStencil() const = 0;
	virtual float4 GetClearColor() const = 0;
};


