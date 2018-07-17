#pragma once

#include "RenderComponent.h"
#include "Graphics/Rendering/Mesh.h"
#include "Graphics/Rendering/Material.h"


class MeshRenderingComponent : public RenderComponent
{
	IMPLEMENT_COMPONENT(MeshRenderingComponent)
	IMPLEMENT_BASE_CLASS(RenderComponent)

public:

	MeshRenderingComponent();
	virtual void Start() override;
	virtual void SubmitFeature(SceneRenderer* Renderer) override;

	inline MATERIAL_PARAMETERS& GetMaterialParameters()
	{
		return RenderedMaterial.GetMaterialParameters();
	}

	Mesh RenderedMesh;
	Material RenderedMaterial;

private:

	IConstantBuffer* m_Constants;

};
