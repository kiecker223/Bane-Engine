#pragma once

#include "RenderComponent.h"
//#include "Graphics/Data/RenderLoop.h"
#include <Graphics/Data/RenderLoop.h>


class MeshRenderingComponent : public RenderComponent
{
	IMPLEMENT_COMPONENT(MeshRenderingComponent)
	IMPLEMENT_BASE_CLASS(RenderComponent)

public:

	MeshRenderingComponent();
	virtual void Start() override;
	virtual void GraphicsUpdate(RenderLoop& Loop) override;

	inline MATERIAL_PARAMETERS GetMaterialParameters()
	{
		if (RenderedMaterial.GetShaderConfiguration() != nullptr)
			return RenderedMaterial.GetMaterialParameters();
		return MATERIAL_PARAMETERS();
	}

	bool		bStaticMesh;
	Mesh*		RenderedMesh;
	Material	RenderedMaterial;

private:

};
