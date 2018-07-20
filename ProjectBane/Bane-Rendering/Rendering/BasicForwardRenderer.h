#pragma once

#include "RendererInterface.h"
#include "Graphics/Interfaces/ApiRuntime.h"
#include <Graphics/Data/Mesh.h>
#include <Graphics/Data/Material.h>


class BasicForwardRenderer : public ISceneRenderer
{
public:

	virtual void Render() override final;

	virtual void Present() override final;

	virtual void Shutdown() override final;

	virtual IDeviceSwapChain* GetSwapChain() { return nullptr; };

	virtual bool SupportsAsyncContexts() { return true; }

	virtual void Submit(const RenderLoop& pRenderLoop) { UNUSED(pRenderLoop); }

private:

};