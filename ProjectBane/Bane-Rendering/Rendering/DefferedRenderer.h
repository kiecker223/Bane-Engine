#pragma once

#include "RendererInterface.h"
#include <Graphics/Interfaces/ApiRuntime.h>
#include <Graphics/Interfaces/ShaderResourceView.h>
#include <Graphics/Data/RenderLoop.h>


class DefferedRenderer : public ISceneRenderer
{
public:
	DefferedRenderer();
	~DefferedRenderer();

	virtual void Initialize(const Window* pWindow) override final;

	virtual void Render() override final;

	virtual void Present() override final;

	virtual void Shutdown() override final;

	virtual IDeviceSwapChain* GetSwapChain() override final;

	virtual bool SupportsAsyncContexts() override final;

	virtual void Submit(const RenderLoop& InRenderLoop) override final;

private:

	void RenderShadows(matrix LightMatrix, IRenderPassInfo* DestRenderPass, IGraphicsCommandContext* ctx);

	void GatherSceneData();
	
	struct
	{
		IVertexBuffer* VB;
		IIndexBuffer* IB;
		IGraphicsPipelineState* Pipeline;
		IShaderResourceTable* Table;
	} m_OnScreenQuad;

	std::vector<RENDER_LOOP_DRAW_COMMIT> m_DrawCommits;

	float Frame = 0.0f;
	
	IConstantBuffer* m_MaterialBuffer;
	IConstantBuffer* m_CameraBuffer;
	IConstantBuffer* m_MeshDataBuffer;
	IConstantBuffer* m_LightBuffer;
	IRuntimeGraphicsDevice* m_Device;
	IRenderPassInfo* m_DefferedPass;
	IConstantBuffer* m_CameraConstants;

	ITexture2D* m_TestTex;
	ITexture2D* m_AlbedoBuffer;
	ITexture2D* m_NormalBuffer;
	ITexture2D* m_PositionBuffer;
	ITexture2D* m_ParameterBuffer;
};

