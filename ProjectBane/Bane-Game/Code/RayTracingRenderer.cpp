#include "RayTracingRenderer.h"

struct ScreenSizes
{
	float x, y, Frame, Padding1;
	//float Frame;
};

ScreenSizes CBufferData = { };

void RayTracingRenderer::Initialize()
{
	m_Device = GetApiRuntime()->GetGraphicsDevice();

	struct Vertex
	{
		struct { float x, y, z; };
		struct { float u, v; };
	};

	Vertex vertices[4] = {
		{ { -1.0f, -1.0f,  0.1f },{ 0.0f, 1.0f } },
		{ { 1.0f, -1.0f,  0.1f },{ 1.0f, 1.0f } },
		{ { 1.0f,  1.0f,  0.1f },{ 1.0f, 0.0f } },
		{ { -1.0f,  1.0f,  0.1f },{ 0.0f, 0.0f } }
	};

	uint indices[6] = {
		0, 3, 2,
		2, 1, 0
	};

	Window* AppWindow = GetApplicationInstance()->GetWindow();
	float WindowWidth = (float)AppWindow->GetWidth();
	float WindowHeight = (float)AppWindow->GetHeight();
	CBufferData = { WindowWidth, WindowHeight, 0.0f, 0.0f };

	m_OnScreenQuad.VB = m_Device->CreateVertexBuffer(sizeof(vertices), (uint8*)vertices);
	m_OnScreenQuad.IB = m_Device->CreateIndexBuffer(sizeof(indices), (uint8*)indices);
	m_OnScreenQuad.Pipeline = GetShaderCache()->LoadGraphicsPipeline("Shaders/RayTracingPS.hlsl");
	m_OnScreenQuad.Table = m_Device->CreateShaderTable(m_OnScreenQuad.Pipeline);
	m_OnScreenQuad.CB = m_Device->CreateConstBuffer<ScreenSizes>();
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();
	ctx->Begin();
	
	memcpy(ctx->Map(m_OnScreenQuad.CB), (void*)&CBufferData, sizeof(ScreenSizes));
	ctx->Unmap(m_OnScreenQuad.CB);

	ctx->End();
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_OnScreenQuad.CB, 0);
}

void RayTracingRenderer::Render()
{
	IGraphicsCommandContext* Ctx = m_Device->GetGraphicsContext();
	Ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	CBufferData.Frame++;

	void* Buff = Ctx->Map(m_OnScreenQuad.CB);
	memcpy(Buff, (void*)&CBufferData, sizeof(ScreenSizes));
	Ctx->Unmap(m_OnScreenQuad.CB);

	Ctx->SetGraphicsPipelineState(m_OnScreenQuad.Pipeline);
	Ctx->SetGraphicsResourceTable(m_OnScreenQuad.Table);
	Ctx->SetVertexBuffer(m_OnScreenQuad.VB);
	Ctx->SetIndexBuffer(m_OnScreenQuad.IB);
	Ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Ctx->DrawIndexed(6, 0, 0);

	Ctx->EndPass();
}

void RayTracingRenderer::Present()
{
	m_Device->GetSwapChain()->Present();
}

void RayTracingRenderer::Shutdown()
{
	delete m_OnScreenQuad.VB;
	delete m_OnScreenQuad.IB;
	delete m_OnScreenQuad.Pipeline;
}
