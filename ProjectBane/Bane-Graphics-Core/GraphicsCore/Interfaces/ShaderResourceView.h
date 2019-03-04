#pragma once
#include "KieckerMath.h"

class IGPUResource;

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

class IConstantBufferView
{
public:
	virtual ~IConstantBufferView() { }
};

class IShaderResourceView
{
public:
	virtual ~IShaderResourceView() { }
};

class IRenderTargetInfo
{
public:
	virtual ~IRenderTargetInfo() { }

	virtual IRenderTargetView** GetRenderTargets() const = 0;
	virtual uint32 GetNumRenderTargets() const = 0;
	virtual IRenderTargetView* GetRenderTarget(uint32 Index) const = 0;
	virtual IDepthStencilView* GetDepthStencil() const = 0;
	virtual fvec4 GetClearColor() const = 0;
};


