#include "RayTracingRenderer.h"

struct ScreenSizes
{
	float x, y, Frame;
	int Bounces;
};

ScreenSizes CBufferData = { };



void RayTracingRenderer::Initialize()
{
	m_Device = GetApiRuntime()->GetGraphicsDevice();

	struct Vertex
	{
		struct { float x, y; };
		struct { float u, v; };
	};

	Vertex vertices[4] = {
		{ { -1.0f, -1.0f },{ 0.0f, 1.0f } },
		{ { 1.0f, -1.0f  },{ 1.0f, 1.0f } },
		{ { 1.0f,  1.0f  },{ 1.0f, 0.0f } },
		{ { -1.0f,  1.0f },{ 0.0f, 0.0f } }
	};

	uint indices[6] = {
		0, 3, 2,
		2, 1, 0
	};

	ZeroMemory(&m_GeometryData, sizeof(m_GeometryData));

	GEOM_SPHERE Sphere = {
		{
			XMFLOAT3(0.3f, 0.1f, 0.7f),
			0.0f,
			0.0f,
			0.0f, 0.0f, 0.0f
		},
		XMFLOAT3(-1.0f, -0.1f, -3.0f),
		0.5f
	};

	AddSphere(Sphere);

	Sphere.Center = XMFLOAT3(1.0f, -0.2f, -2.7f);
	Sphere.Material.Color = XMFLOAT3(0.6f, 0.9f, 0.1f);
	Sphere.Material.Roughness = 0.0f;
	Sphere.Radius = 0.5f;

	AddSphere(Sphere);

	GEOM_PLANE Plane = {
		{
			XMFLOAT3(0.9f, 0.7f, 0.1f),
			0.0f,
			0.0f,
			0.0f, 0.0f, 0.0f
		},
		XMFLOAT3(0.0f, -0.5f, 0.0f),
		1.0f,
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		0.0f
	};

	AddPlane(Plane);

	Window* AppWindow = GetApplicationInstance()->GetWindow();
	float WindowWidth = (float)AppWindow->GetWidth();
	float WindowHeight = (float)AppWindow->GetHeight();
	CBufferData = { WindowWidth, WindowHeight, 0.0f, 3 };

	m_OnScreenQuad.VB = m_Device->CreateVertexBuffer(sizeof(vertices), (uint8*)vertices);
	m_OnScreenQuad.IB = m_Device->CreateIndexBuffer(sizeof(indices), (uint8*)indices);
	m_OnScreenQuad.Pipeline = GetShaderCache()->LoadGraphicsPipeline("Shaders/RayTracingPS.hlsl");
	m_OnScreenQuad.Table = m_Device->CreateShaderTable(m_OnScreenQuad.Pipeline);
	m_OnScreenQuad.CBs[0] = m_Device->CreateConstBuffer<ScreenSizes>();
	m_OnScreenQuad.CBs[1] = m_Device->CreateConstBuffer<GEOMETRY_DATA>();

	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();
	ctx->Begin();
	
	memcpy(ctx->Map(m_OnScreenQuad.CBs[0]), (void*)&CBufferData, sizeof(ScreenSizes));
	ctx->Unmap(m_OnScreenQuad.CBs[0]);
	memcpy(ctx->Map(m_OnScreenQuad.CBs[1]), (void*)&m_GeometryData, sizeof(GEOMETRY_DATA));
	ctx->Unmap(m_OnScreenQuad.CBs[1]);
	ctx->End();
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_OnScreenQuad.CBs[0], 0);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_OnScreenQuad.CBs[1], 1);
}

void RayTracingRenderer::Render()
{
	IGraphicsCommandContext* Ctx = m_Device->GetGraphicsContext();

	Ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	CBufferData.Frame++;

	void* Buff = Ctx->Map(m_OnScreenQuad.CBs[0]);
	memcpy(Buff, (void*)&CBufferData, sizeof(ScreenSizes));
	Ctx->Unmap(m_OnScreenQuad.CBs[0]);

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

void RayTracingRenderer::AddPlane(GEOM_PLANE InPlane)
{
	m_GeometryData.Planes[m_GeometryData.NumPlanes] = InPlane;
	m_GeometryData.NumPlanes++;
}

void RayTracingRenderer::AddSphere(GEOM_SPHERE InSphere)
{
	m_GeometryData.Spheres[m_GeometryData.NumSpheres] = InSphere;
	m_GeometryData.NumSpheres++;
}

void RayTracingRenderer::AddBox(GEOM_BOX InBox)
{
	m_GeometryData.Boxes[m_GeometryData.NumBoxes] = InBox;
	m_GeometryData.NumBoxes++;
}

void RayTracingRenderer::AddPointLight(GEOM_POINT_LIGHT InPointLight)
{
	m_GeometryData.PointLights[m_GeometryData.NumPointLights] = InPointLight;
	m_GeometryData.NumPointLights++;
}

void RayTracingRenderer::AddDirectionalLight(GEOM_DIRECTIONAL_LIGHT InDirLight)
{
	m_GeometryData.DirectionalLights[m_GeometryData.NumDirectionalLights] = InDirLight;
	m_GeometryData.NumDirectionalLights++;
}
