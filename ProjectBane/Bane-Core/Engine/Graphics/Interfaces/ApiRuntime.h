#pragma once
#include "Core/Common.h"
#include "GraphicsDevice.h"
#include "SwapChainInterface.h"


typedef enum EAPI {
	API_D3D12,
	API_VULKAN,
	API_D3D11,
	API_OPENGL_4_3,
	API_D3D9
} EAPI;

class ApiRuntime
{
public:

	static ApiRuntime* Get() { return Runtime; }
	static void SetApiRuntime(ApiRuntime* pRuntime) { Runtime = pRuntime; }
	static void Shutdown() { Runtime->Destroy(); delete Runtime; }
	static void CreateRuntime();

	virtual EFORMAT GetBackBufferFormat() const { return FORMAT_R8G8B8A8_UNORM; }
	virtual EAPI GetApiType() const = 0;
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;
	virtual bool SupportsAsyncContexts() const = 0;

	virtual IRuntimeGraphicsDevice* GetGraphicsDevice() = 0;
	virtual IDeviceSwapChain* GetSwapChain() = 0;

private:

	static ApiRuntime* Runtime;

};

ForceInline ApiRuntime* GetApiRuntime()
{
	return ApiRuntime::Get();
}

