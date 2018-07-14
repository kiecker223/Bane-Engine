#include "SkyboxComponent.h"
#include "Graphics/IO/TextureCache.h"
#include "Graphics/IO/ShaderCache.h"
#include "Graphics/Rendering/Mesh.h"


SkyboxComponent::~SkyboxComponent()
{
	delete m_Skybox;
	delete m_VertexBuffer;
	delete m_Table;
	delete m_Pipeline;
}

void SkyboxComponent::SubmitFeature(SceneRenderer* Renderer)
{
	float SkyboxVertices[] = {
		// positions        
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
							
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
							
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
							
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
							
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
							
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	//Mesh SkyboxMesh = Mesh::CreateSphere(10);
	m_VertexBuffer = GetApiRuntime()->GetGraphicsDevice()->CreateVertexBuffer(sizeof(SkyboxVertices), (uint8*)SkyboxVertices);
	//m_IndexBuffer = GetApiRuntime()->GetGraphicsDevice()->CreateIndexBuffer(SkyboxMesh.GetIndices().size() * sizeof(uint), (uint8*)SkyboxMesh.GetIndices().data());
	//m_IndexCount = SkyboxMesh.GetIndexCount();

	Renderer->SetSkybox(this);
}

void SkyboxComponent::SetSkybox(const std::string& Skybox)
{
	m_Skybox = GetTextureCache()->LoadCubemap(Skybox);
	m_SkyboxName = Skybox;
	
	GetApiRuntime()->GetGraphicsDevice()->CreateShaderResourceView(m_Table, m_Skybox, 0);
}

void SkyboxComponent::SetSkybox(
	const std::string& Name,
	const std::string& NX,
	const std::string& NY,
	const std::string& NZ,
	const std::string& PX,
	const std::string& PY,
	const std::string& PZ)
{
	m_Skybox = GetTextureCache()->LoadCubemap(Name, NX, NY, NZ, PX, PY, PZ);
}

void SkyboxComponent::SetSkyboxShader(const std::string& ShaderName)
{
	m_Pipeline = GetShaderCache()->LoadGraphicsPipeline(ShaderName);
	m_Table = GetApiRuntime()->GetGraphicsDevice()->CreateShaderTable(m_Pipeline);
}

void SkyboxComponent::SetCameraConstants(IConstantBuffer* Buffer)
{
	IRuntimeGraphicsDevice* Device = GetApiRuntime()->GetGraphicsDevice();
	Device->CreateShaderResourceView(m_Table, Buffer, 0);
	Device->CreateShaderResourceView(m_Table, m_Skybox, 0);
}

void SkyboxComponent::Draw(IGraphicsCommandContext* Ctx)
{
	Ctx->SetGraphicsPipelineState(m_Pipeline);
	Ctx->SetGraphicsResourceTable(m_Table);
	Ctx->SetVertexBuffer(m_VertexBuffer);
	//Ctx->SetIndexBuffer(m_IndexBuffer);
	Ctx->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Ctx->Draw(108, 0);
	//Ctx->DrawIndexed(m_IndexCount, 0, 0);
}

