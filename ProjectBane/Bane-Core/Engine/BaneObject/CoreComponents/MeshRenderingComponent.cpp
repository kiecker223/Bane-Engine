#include "MeshRenderingComponent.h"
#include "Graphics/Interfaces/ApiRuntime.h"


MeshRenderingComponent::MeshRenderingComponent()
{

}

void MeshRenderingComponent::Start()
{
}

void MeshRenderingComponent::SubmitFeature(SceneRenderer* Renderer)
{
	m_Constants = GetApiRuntime()->GetGraphicsDevice()->CreateConstantBuffer(204);
	Renderer->AddBasicMesh(RenderedMesh, RenderedMaterial, GetOwner(), m_Constants);

	
	//if (!(RenderedMaterial.RenderFlags & RENDER_FLAG_NO_SHADOWS) == RENDER_FLAG_NO_SHADOWS)
	//{
	//}
}

