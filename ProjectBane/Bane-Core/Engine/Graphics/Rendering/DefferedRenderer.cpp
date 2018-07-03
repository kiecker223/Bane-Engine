#include "DefferedRenderer.h"
#include "Application/Window.h"
#include "Application/Application.h"
#include "../Interfaces/ApiRuntime.h"
#include "Core/KieckerMath.h"
#include "Graphics/IO/ShaderCache.h"
#include "Graphics/IO/TextureCache.h"
#include "BaneObject/CoreComponents/CameraComponent.h"
#include "BaneObject/CoreComponents/SkyboxComponent.h"


typedef struct DEFFERED_CAMERA_CONSTANTS {
	matrix Model, View, Projection;
	float3 CameraPosition;
} DEFFERED_CAMERA_CONSTANTS;

DEFFERED_CAMERA_CONSTANTS CameraConstants = { };
matrix SkyboxModel;
DefferedRenderer::DefferedRenderer()
{
}

DefferedRenderer::~DefferedRenderer()
{
}

void DefferedRenderer::Initialize()
{
	m_DrawItems.reserve(100);
	m_Lights.reserve(100);

	m_Device = GetApiRuntime()->GetGraphicsDevice();
	uint Height = GetApplicationInstance()->GetWindow()->GetHeight();
	uint Width = GetApplicationInstance()->GetWindow()->GetWidth();

	m_AlbedoBuffer		= m_Device->CreateTexture2D(Width, Height, FORMAT_R8G8B8A8_UNORM,	  (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_NormalBuffer		= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_PositionBuffer	= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);
	m_ParameterBuffer	= m_Device->CreateTexture2D(Width, Height, FORMAT_R32G32B32A32_FLOAT, (TEXTURE_USAGE_SHADER_RESOURCE | TEXTURE_USAGE_RENDER_TARGET), nullptr);

	const IRenderTargetView* Views[4] =
	{
		m_Device->CreateRenderTargetView(m_AlbedoBuffer),
		m_Device->CreateRenderTargetView(m_NormalBuffer),
		m_Device->CreateRenderTargetView(m_PositionBuffer),
		m_Device->CreateRenderTargetView(m_ParameterBuffer)
	};

	m_DefferedPass = m_Device->CreateRenderPass(&Views[0], 4, m_Device->GetDefaultDepthStencil(), float4(0.0f, 0.0f, 0.0f, 0.0f));

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

	m_OnScreenQuad.VB = m_Device->CreateVertexBuffer(sizeof(vertices), (uint8*)vertices);
	m_OnScreenQuad.IB = m_Device->CreateIndexBuffer(sizeof(indices), (uint8*)indices);
	m_OnScreenQuad.Pipeline = GetShaderCache()->LoadGraphicsPipeline("Shaders/DefferredLighting.hlsl");
	m_OnScreenQuad.Table = m_Device->CreateShaderTable(m_OnScreenQuad.Pipeline);
	m_OnScreenQuad.PointSampler = m_Device->GetDefaultSamplerState();
	SAMPLER_DESC SamplerDesc = CreateDefaultSamplerDesc();
	m_OnScreenQuad.SkySampler = m_Device->CreateSamplerState(SamplerDesc);
	
	m_LightCBData.Frame = 0.0f;

	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_AlbedoBuffer, 0);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_NormalBuffer, 1);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_PositionBuffer, 2);
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_ParameterBuffer, 3);

	m_CameraConstants = m_Device->CreateConstBuffer<DEFFERED_CAMERA_CONSTANTS>();
	m_LightBuffer = m_Device->CreateConstBuffer<DEFFERED_RENDERER_LIGHT_DATA>();
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, m_LightBuffer, 0);
	m_Device->CreateSamplerView(m_OnScreenQuad.Table, m_OnScreenQuad.SkySampler, 1);
}

void DefferedRenderer::Render()
{
	Frame++;
	IGraphicsCommandContext* ctx = m_Device->GetGraphicsContext();
	ctx->BeginPass(m_DefferedPass);

	float3 CameraPosition = MainCamera->GetTransform()->GetPosition();
	{
		CameraConstants.View = MainCamera->GetLookAt();
		CameraConstants.Projection = MainCamera->GetProjection();
		CameraConstants.CameraPosition = CameraPosition;
	}

	for (uint i = 0; i < m_DrawItems.size(); i++)
	{
		DrawItem& Item = m_DrawItems[i];

		void* Buff = ctx->Map(Item.CB);
		CameraConstants.Model = Item.OwningEntity->GetTransform()->GetMatrix();
		memcpy(Buff, (void*)&CameraConstants, sizeof(DEFFERED_CAMERA_CONSTANTS));
		ctx->Unmap(Item.CB);
		m_Device->CreateShaderResourceView(Item.RenderMaterial.GetTable(), Item.CB, 0);

		Item.RenderMaterial.UpdateMaterialParameters(ctx);
		Item.RenderMaterial.Bind(ctx);
		ctx->SetVertexBuffer(Item.RenderMesh.GetVertexBuffer());
		ctx->SetIndexBuffer(Item.RenderMesh.GetIndexBuffer());
		ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ctx->DrawIndexed(Item.RenderMesh.GetIndexCount(), 0, 0);
	}

	if (Skybox)
	{
		void* Buff = ctx->Map(m_CameraConstants);
		CameraConstants.Model = matTransformation(CameraPosition, Quaternion(), float3(5.f, 5.f, 5.f));
		memcpy(Buff, (void*)&CameraConstants, sizeof(DEFFERED_CAMERA_CONSTANTS));
		ctx->Unmap(m_CameraConstants);
		Skybox->SetCameraConstants(m_CameraConstants);
		Skybox->Draw(ctx);
	}
	ctx->EndPass(); // So right now its executing

	ctx->BeginPass(m_Device->GetBackBufferTargetPass());
	GatherLights();
	m_LightCBData.Frame = Frame;
	void* LightBuff = ctx->Map(m_LightBuffer);
	memcpy(LightBuff, (void*)&m_LightCBData, sizeof(DEFFERED_RENDERER_LIGHT_DATA));
	ctx->Unmap(m_LightBuffer);
	ctx->SetGraphicsPipelineState(m_OnScreenQuad.Pipeline);
	ctx->SetGraphicsResourceTable(m_OnScreenQuad.Table);
	ctx->SetVertexBuffer(m_OnScreenQuad.VB);
	ctx->SetIndexBuffer(m_OnScreenQuad.IB);
	ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ctx->DrawIndexed(6, 0, 0);

	ZeroMemory(&m_LightCBData, sizeof(DEFFERED_RENDERER_LIGHT_DATA));

	ctx->EndPass();
}

void DefferedRenderer::Present()
{
	GetSwapChain()->Present();
}

void DefferedRenderer::Shutdown()
{

}

IDeviceSwapChain* DefferedRenderer::GetSwapChain()
{
	return m_Device->GetSwapChain();
}

bool DefferedRenderer::SupportsAsyncContexts()
{
	EAPI Api = GetApiRuntime()->GetApiType();
	return (Api == API_D3D12 || Api == API_VULKAN);
}

void DefferedRenderer::AddBasicMesh(const Mesh& InMesh, const Material& InMaterial, Entity* Owner, IConstantBuffer* Constants)
{
	m_DrawItems.push_back({ InMesh, InMaterial, Owner, Constants });
}

void DefferedRenderer::AddCamera(CameraComponent* InCamera)
{
	Cameras.push_back(InCamera);
	if (InCamera->GetPriority() == 1)
	{
		if (MainCamera != nullptr)
		{
			BaneLog() << "Uhh, not supposed to be another camera here!";
			return;
		}
		MainCamera = InCamera;
	}
}

void DefferedRenderer::AddLight(LightComponent* InLight)
{
	m_Lights.push_back(InLight->GetOwner());
}

void DefferedRenderer::RenderShadows(matrix LightMatrix, IRenderPassInfo* DestRenderPass, IGraphicsCommandContext* ctx)
{

}

void DefferedRenderer::GatherLights()
{
	m_LightCBData.CameraPosition = MainCamera->GetTransform()->GetPosition();
	for (uint i = 0; i < m_Lights.size(); i++)
	{
		LightComponent* Comp = m_Lights[i]->GetComponent<LightComponent>();
		switch (Comp->GetLightType())
		{
			case LIGHT_TYPE_DIRECTIONAL:
			{
				DIRECTIONAL_LIGHT_DATA& DirLight = m_LightCBData.DirectionalLights[m_LightCBData.NumDirectionalLights];
				DirLight = Comp->GetDirectionalLight();
				m_LightCBData.NumDirectionalLights++;
			} break;
			case LIGHT_TYPE_POINT:
			{
				POINT_LIGHT_DATA& PointLight = m_LightCBData.PointLights[m_LightCBData.NumPointLights];
				PointLight = Comp->GetPointLight();
				m_LightCBData.NumPointLights++;
			} break;
			case LIGHT_TYPE_SPOT:
			{
				SPOTLIGHT_DATA& SpotLight = m_LightCBData.SpotLights[m_LightCBData.NumSpotLights];
				SpotLight = Comp->GetSpotLight();
				m_LightCBData.NumSpotLights++;
			} break;
		}
	}
}

void DefferedRenderer::SetSkybox(SkyboxComponent* InSkybox)
{
	Skybox = InSkybox;
	m_Device->CreateShaderResourceView(m_OnScreenQuad.Table, InSkybox->GetSkyboxTexture(), 4);
}
