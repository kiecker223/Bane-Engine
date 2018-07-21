#include "D3D12SwapChain.h"
#include "D3D12GraphicsDevice.h"


bool D3D12SwapChain::Initialize(IDXGIFactory2* Factory, IDXGIAdapter* InAdapter, ID3D12CommandQueue* MainQueue, const Window* RenderingWindow)
{
	Adapter = InAdapter;
	const uint Width = RenderingWindow->GetWidth();
	const uint Height = RenderingWindow->GetHeight();
	HWND WindowHandle = RenderingWindow->GetHandle()->GetNativeHandle<HWND>();
	DXGI_SWAP_CHAIN_DESC1 Desc = { };
	Desc.Width = Width;
	Desc.Height = Height;
	Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc.Stereo = FALSE;
	Desc.SampleDesc = { 1, 0 };
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.BufferCount = 3;
	Desc.Scaling = DXGI_SCALING_STRETCH;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	Desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	Desc.Flags = 0;

	IDXGISwapChain1* TempSwapChain = nullptr;
	HRESULT HRes = Factory->CreateSwapChainForHwnd(MainQueue, WindowHandle, &Desc, nullptr, nullptr, &TempSwapChain);

	TempSwapChain->QueryInterface(&SwapChain);

	return SUCCEEDED(HRes);
}

void D3D12SwapChain::SetSwapInterval(uint SyncInterval)
{
	SyncRate = SyncInterval;
}

void D3D12SwapChain::Present()
{
	// Gross
	Device->GetComputeQueue().StallForFinish();
	Device->GetCommandQueue(COMMAND_CONTEXT_TYPE_GRAPHICS).StallForFinish();
	SwapChain->Present(SyncRate, 0);
	Device->UpdateCurrentFrameInfo();
}

IRuntimeGraphicsDevice* D3D12SwapChain::GetParent()
{
	return Device;
}
