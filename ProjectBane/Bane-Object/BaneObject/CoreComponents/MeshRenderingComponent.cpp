#include "MeshRenderingComponent.h"
#include "../Entity/Transform.h"
#include <Graphics/Interfaces/ApiRuntime.h>
#include "../Scene/SceneManager.h"
#include "../Entity/Entity.h"


MeshRenderingComponent::MeshRenderingComponent() :
	bStaticMesh(false),
	RenderedMesh(nullptr)
{
}

void MeshRenderingComponent::Start()
{
	if (bStaticMesh)
	{
	}
}

void MeshRenderingComponent::GraphicsUpdate(RenderLoop& Loop)
{
	Loop.DrawMesh(RenderedMesh, &RenderedMaterial, GetOwner()->GetMatrixAffectedByParents());
}
