#include "LightComponent.h"


void LightComponent::GraphicsUpdate(RenderLoop& Loop)
{
	UNUSED(Loop);
//	Renderer->AddLight(this);
}

void LightComponent::SetLightType(ELIGHT_TYPE LightType)
{
	m_Type = LightType;
}

DIRECTIONAL_LIGHT_DATA LightComponent::GetDirectionalLight() const
{
	BANE_CHECK(m_Type == LIGHT_TYPE_DIRECTIONAL);
	DIRECTIONAL_LIGHT_DATA Result = {};
	Result.Color = Color;
	Result.Direction = fromDouble3(GetOwner()->GetTransform()->GetForward());
	Result.Range = Range;
	Result.Intensity = Intensity;
	return Result;
}

POINT_LIGHT_DATA LightComponent::GetPointLight() const
{
	//BANE_CHECK(m_Type == LIGHT_TYPE_POINT);
	POINT_LIGHT_DATA Result = {};
	Result.Color = Color;
	Result.Position = fromDouble3(GetOwner()->GetTransform()->GetPosition());
	Result.Intensity = Intensity;
	Result.Range = Range;
	return Result;
}

SPOTLIGHT_DATA LightComponent::GetSpotLight() const
{
	//BANE_CHECK(m_Type == LIGHT_TYPE_SPOT);
	SPOTLIGHT_DATA Result = {};
	Result.Color = Color;
	Result.Direction = fromDouble3(GetOwner()->GetTransform()->GetForward());
	Result.Position = fromDouble3(GetOwner()->GetTransform()->GetPosition());
	Result.Range = Range;
	Result.SpotAngle = SpotAngle;
	Result.Intensity = Intensity;
	return Result;
}

