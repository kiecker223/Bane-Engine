#pragma once

#include "../Entity/ComponentBase.h"
#include "Graphics/Rendering/RendererInterface.h"

// The only thing significant about this class is that it
// helps with sending things to the renderer

class RenderComponent : public Component
{
	IMPLEMENT_COMPONENT(RenderComponent)

public:

	virtual bool IsRenderComponent() const override { return true; }

	virtual void SubmitFeature(SceneRenderer* Renderer) = 0;

};


