#pragma once

#include "RendererInterface.h"
#include <GraphicsCore/Interfaces/ApiRuntime.h>
#include <mutex>
#include <GraphicsCore/Data/Mesh.h>
#include <GraphicsCore/Data/Material.h>


class BasicForwardRenderer : public ISceneRenderer
{
public:

	virtual void Initialize(const Window* pWindow) override;

	virtual void StartRenderThread() override;

	virtual void Render() override final;

	virtual void Present() override final;

	virtual void Shutdown() override final;

	virtual IDeviceSwapChain* GetSwapChain() { return m_Device->GetSwapChain(); };

	virtual bool SupportsAsyncContexts() { return true; }

	virtual void Submit(RenderLoop& pRenderLoop) override;

private:

	void GatherSceneData();

	bool m_bIsRendering;
	std::vector<RenderMeshMaterialBucket::BucketItem> m_DrawList;
	//std::mutex m_RenderSubmitLock;
	IGraphicsDevice* m_Device;
	IConstantBuffer* m_CameraConstants;
	IConstantBuffer* m_MaterialConstants;
	IConstantBuffer* m_MeshDataBuffer;
	IConstantBuffer* m_LightBuffer;
	IGraphicsPipelineState* m_ImmediateGeometryPS;
	RenderLoop* m_pRenderLoop;
};