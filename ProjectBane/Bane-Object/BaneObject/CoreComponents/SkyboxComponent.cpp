#include "SkyboxComponent.h"
#include "Graphics/IO/TextureCache.h"
#include "Graphics/IO/ShaderCache.h"


SkyboxComponent::~SkyboxComponent()
{
	delete m_Skybox;
	delete m_VertexBuffer;
}

void SkyboxComponent::GraphicsUpdate(RenderLoop& Loop)
{
	UNUSED(Loop);
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
}

void SkyboxComponent::SetSkybox(const std::string& Skybox)
{
	m_Skybox = GetTextureCache()->LoadCubemap(Skybox);
	m_SkyboxName = Skybox;
	
	
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
	UNUSED(ShaderName);
}

