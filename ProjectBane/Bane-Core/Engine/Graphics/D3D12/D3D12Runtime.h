#pragma once

#include "../Interfaces/ApiRuntime.h"

class D3D12Runtime : public ApiRuntime
{
public:

	virtual EAPI GetApiType() const final override { return EAPI::API_D3D12; }

	virtual void Initialize() final override;
	virtual void Destroy() final override;
	virtual bool SupportsAsyncContexts() const final override { return true; }
	
	virtual IRuntimeGraphicsDevice* GetGraphicsDevice() final override { return (IRuntimeGraphicsDevice*)m_Device; }
	virtual IDeviceSwapChain* GetSwapChain() final override { return (IDeviceSwapChain*)m_SwapChain; }

private:

	class D3D12GraphicsDevice* m_Device;
	class D3D12SwapChain* m_SwapChain;
};

