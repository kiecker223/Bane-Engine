#pragma once

#include "Common.h"

class IRuntimeGraphicsDevice;

class IDeviceSwapChain
{
public:

	virtual ~IDeviceSwapChain() { }

	virtual void SetSwapInterval(uint32 SyncInterval) = 0;

	virtual void Present() = 0;

	virtual IRuntimeGraphicsDevice* GetParent() = 0;

};
