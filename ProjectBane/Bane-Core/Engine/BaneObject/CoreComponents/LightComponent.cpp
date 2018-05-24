#include "LightComponent.h"


void LightComponent::SubmitFeature(SceneRenderer* Renderer)
{
	Renderer->AddLight(this);
}

void LightComponent::SetLightType(ELIGHT_TYPE LightType)
{
	ELIGHT_TYPE PrevType = m_Type;
	if (LightType == PrevType)
	{
		return;
	}
	m_Type = LightType;
}

