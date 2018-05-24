#pragma once

#include "Engine/Engine.h"


class RayTracingRenderer : public SceneRenderer
{
public:

	virtual void Initialize() override final;
	virtual void Render() override final;
	virtual void Present() override final;
	virtual void Shutdown() override final;
	virtual IDeviceSwapChain* GetSwapChain() override final
	{
		return m_Device->GetSwapChain();
	}

private:

	struct
	{
		IVertexBuffer* VB;
		IIndexBuffer* IB;
		IGraphicsPipelineState* Pipeline;
		IShaderResourceTable* Table;
		IConstantBuffer* CB;
	} m_OnScreenQuad;


	IRuntimeGraphicsDevice* m_Device;

};


