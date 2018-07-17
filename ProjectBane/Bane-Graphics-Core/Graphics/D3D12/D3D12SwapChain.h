#pragma once

#include <dxgi1_5.h>
#include <d3d12.h>
#include "../Interfaces/SwapChainInterface.h"
#include <System/Window.h>

class D3D12GraphicsDevice;

typedef void(D3D12GraphicsDevice::*PFNPresentCallback)();

class D3D12SwapChain : public IDeviceSwapChain
{
public:

	~D3D12SwapChain()
	{
		SwapChain->Release();
		Adapter->Release();
	}

	bool Initialize(IDXGIFactory2* Factory, IDXGIAdapter* Adapter, ID3D12CommandQueue* MainQueue, Window* RenderingWindow);

	virtual void SetSwapInterval(uint SyncInterval) override final;

	virtual void Present() override final;

	D3D12GraphicsDevice* Device;

	IDXGISwapChain4* SwapChain;
	IDXGIAdapter* Adapter;

	uint SyncRate;

};

