#pragma once
#include "KieckerMath.h"

class IGPUResource;


class IShaderResourceTable
{
public:
	virtual ~IShaderResourceTable() { }
	
	virtual uint32 GetNumConstantBuffers() const = 0;
	virtual uint32 GetNumTextures() const = 0;
	virtual uint32 GetNumSamplers() const = 0;
	virtual uint32 GetNumUnorderedAccessViews() const = 0;

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
	virtual uint32 GetNumRenderTargets() const = 0;
	virtual IRenderTargetView* GetRenderTarget(uint32 Index) const = 0;
	virtual IDepthStencilView* GetDepthStencil() const = 0;
	virtual float4 GetClearColor() const = 0;
};


