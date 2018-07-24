#include "D3D12Runtime.h"
#include "D3D12GraphicsDevice.h"
#include "D3D12SwapChain.h"
#include "Common.h"
#include "Platform/System/Logging/Logger.h"

#define USE_WARP 0
#define USE_DEBUG_INTERFACE 0

#define PLEASE_HELP_I_BROKE_THE_THING_AGAIN 1

void D3D12Runtime::Initialize(const Window* pWindow)
{
	m_SwapChain = new D3D12SwapChain();

#if USE_DEBUG_INTERFACE || PLEASE_HELP_I_BROKE_THE_THING_AGAIN
	ID3D12Debug* Debug;
	D3D12GetDebugInterface(IID_PPV_ARGS(&Debug));
	Debug->EnableDebugLayer();
	Debug->Release();
#endif

	IDXGIFactory4* DxgiFactory = nullptr;
	CreateDXGIFactory(IID_PPV_ARGS(&DxgiFactory));

	IDXGIAdapter* Adapter = nullptr;

#if USE_WARP || PLEASE_HELP_I_BROKE_THE_THING_AGAIN
	DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&Adapter));
#else
	DxgiFactory->EnumAdapters(0, &Adapter);
#endif

	DXGI_ADAPTER_DESC AdapterDesc;
	Adapter->GetDesc(&AdapterDesc);

	BaneLog() << AdapterDesc.Description << END_LINE;
	
	ID3D12Device1* Device = nullptr;
	D3D12ERRORCHECK(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device)));

	D3D12_COMMAND_QUEUE_DESC CmdDesc = { D3D12_COMMAND_LIST_TYPE_DIRECT, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
	ID3D12CommandQueue* CommandQueue = nullptr;
	Device->CreateCommandQueue(&CmdDesc, IID_PPV_ARGS(&CommandQueue));

	m_SwapChain->Initialize(DxgiFactory, Adapter, CommandQueue, pWindow);
	m_Device = new D3D12GraphicsDevice(m_SwapChain, pWindow, Device, CommandQueue);
}

void D3D12Runtime::Destroy()
{
	delete m_Device;
}
