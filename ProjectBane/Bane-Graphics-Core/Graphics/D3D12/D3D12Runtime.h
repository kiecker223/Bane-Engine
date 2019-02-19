#pragma once

#include "../Interfaces/ApiRuntime.h"
#include <Platform/System/Window.h>

class D3D12Runtime : public ApiRuntime
{
public:

	virtual EAPI GetApiType() const final override { return EAPI::API_D3D12; }

	virtual void Initialize(const Window* pWindow) final override;
	virtual void Destroy() final override;
	virtual bool SupportsAsyncContexts() const final override { return true; }
	
	virtual IGraphicsDevice* GetGraphicsDevice() final override { return (IGraphicsDevice*)m_Device; }
	virtual IDeviceSwapChain* GetSwapChain() final override { return (IDeviceSwapChain*)m_SwapChain; }

private:

	class D3D12GraphicsDevice* m_Device;
	class D3D12SwapChain* m_SwapChain;
};

