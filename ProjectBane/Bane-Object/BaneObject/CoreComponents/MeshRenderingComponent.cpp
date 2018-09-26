#include "MeshRenderingComponent.h"
#include "../Entity/Transform.h"
#include <Graphics/Interfaces/ApiRuntime.h>


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
	auto* XForm = GetTransform();
	Loop.AddDrawable(RenderedMesh, RenderedMaterial, XForm->GetPosition(), XForm->GetScale(), XForm->GetRotation());
}




