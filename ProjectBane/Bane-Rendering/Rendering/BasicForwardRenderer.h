#pragma once

#include "RendererInterface.h"
#include "Graphics/Interfaces/ApiRuntime.h"
#include <mutex>
#include <Graphics/Data/Mesh.h>
#include <Graphics/Data/Material.h>


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

	virtual void Submit(const RenderLoop& pRenderLoop) override;

private:

	void GatherSceneData();

	bool m_bIsRendering;
	//std::mutex m_RenderSubmitLock;
	IRuntimeGraphicsDevice* m_Device;
	IConstantBuffer* m_CameraConstants;
	IConstantBuffer* m_MaterialConstants;
	IConstantBuffer* m_MeshDataBuffer;
	IConstantBuffer* m_LightBuffer;
	IGraphicsPipelineState* m_ImmediateGeometryPS;
	std::vector<IVertexBuffer*> m_ImmediateGeometry;
	std::vector<RENDER_LOOP_DRAW_COMMIT> m_Commits;

};