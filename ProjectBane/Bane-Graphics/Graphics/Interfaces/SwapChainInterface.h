#pragma once

#include "Common.h"



class IDeviceSwapChain
{
public:

	virtual ~IDeviceSwapChain() { }

	virtual void SetSwapInterval(uint SyncInterval) = 0;

	virtual void Present() = 0;

};
