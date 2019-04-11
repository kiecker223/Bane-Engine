#pragma once

#include "KieckerMath.h"
#include "GraphicsCore/Interfaces/GraphicsCommandList.h"
#include "GraphicsCore/Data/RenderLoop.h"
#include <Platform/System/Window.h>
#include <vector>
#include <thread>

class Scene;
class Mesh;
class Material;
class IGraphicsCommandContext;
class IDeviceSwapChain;
class SkyboxComponent;


class ISceneRenderer
{
public:

	virtual void Submit(RenderLoop& pRenderLoop) = 0;

	virtual void StartRenderThread() = 0;

	virtual void Render() = 0;

	virtual void Initialize(const Window* pWindow) = 0;

	virtual void Present() = 0;

	virtual void Shutdown() = 0;

	virtual IDeviceSwapChain* GetSwapChain() = 0;

	virtual bool SupportsAsyncContexts() { return false; }

protected:

	std::thread RenderThreadHandle;
};
