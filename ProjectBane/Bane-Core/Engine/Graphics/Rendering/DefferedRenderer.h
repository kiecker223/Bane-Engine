#pragma once

#include "Engine/Graphics/Rendering/RendererInterface.h"
#include "../Interfaces/ApiRuntime.h"
#include "../Interfaces/ShaderResourceView.h"
#include "Mesh.h"
#include "Material.h"
#include "BaneObject/CoreComponents/LightComponent.h"


#define MAX_LIGHTS 30

class DefferedRenderer : public SceneRenderer
{
public:
	DefferedRenderer();
	~DefferedRenderer();

	virtual void AllocateScene(Scene* pScene) override final { }

	virtual void DumpCurrentScene() override final { }

	virtual void Initialize() override final;

	virtual void Render() override final;

	virtual void Present() override final;

	virtual void Shutdown() override final;

	virtual Scene* GetScene() override final { return nullptr; }

	virtual IDeviceSwapChain* GetSwapChain() override final;

	virtual bool SupportsAsyncContexts() override final;

	virtual void AddBasicMesh(const Mesh& InMesh, const Material& InMaterial, Entity* Owner, IConstantBuffer* Constants) override final;

	virtual void AddCamera(class CameraComponent* InCamera) override final;

	virtual void AddLight(LightComponent* InLight) override final;

	virtual void SetSkybox(SkyboxComponent* InSkybox) override final;

private:

	void RenderShadows(matrix LightMatrix, IRenderPassInfo* DestRenderPass, IGraphicsCommandContext* ctx);

	void GatherLights();

	struct DrawItem
	{
		Mesh RenderMesh;
		Material RenderMaterial;
		Entity* OwningEntity;
		IConstantBuffer* CB;
	};

	std::vector<DrawItem> m_DrawItems;

	uint m_MaxLights;
	std::vector<LightComponent*> m_Lights;

	struct
	{
		IVertexBuffer* VB;
		IIndexBuffer* IB;
		IGraphicsPipelineState* Pipeline;
		IShaderResourceTable* Table;
		ISamplerState* PointSampler;
		ISamplerState* SkySampler;
	} m_OnScreenQuad;

	typedef struct DEFFERED_RENDERER_LIGHT_DATA
	{
		float3				AmbientColor;
		float					Frame = 0;
		float3				CameraPosition;
		float					Padding02;
		DIRECTIONAL_LIGHT_DATA	DirectionalLights[MAX_LIGHTS];
		POINT_LIGHT_DATA		PointLights[MAX_LIGHTS];
		SPOTLIGHT_DATA			SpotLights[MAX_LIGHTS];
		uint					NumDirectionalLights;
		uint					NumPointLights;
		uint					NumSpotLights;
		uint					Padding03;
	} DEFFERED_RENDERER_LIGHT_DATA;

	float Frame = 0.0f;
	DEFFERED_RENDERER_LIGHT_DATA m_LightCBData;
	
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

