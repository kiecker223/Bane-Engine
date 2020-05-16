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



