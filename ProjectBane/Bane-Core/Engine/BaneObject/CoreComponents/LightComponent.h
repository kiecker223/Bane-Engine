#pragma once

#include "RenderComponent.h"
#include "Graphics/Rendering/LightData.h"


typedef enum ELIGHT_TYPE {
	LIGHT_TYPE_NONE,
	LIGHT_TYPE_DIRECTIONAL,
	LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT
} ELIGHT_TYPE;

class LightComponent : public RenderComponent
{
	IMPLEMENT_BASE_CLASS(RenderComponent)
	IMPLEMENT_COMPONENT(LightComponent)
public:

	LightComponent() : m_Type(LIGHT_TYPE_NONE) { }

	void SubmitFeature(SceneRenderer* Renderer);

	void SetLightType(ELIGHT_TYPE LightType);

	ForceInline ELIGHT_TYPE GetLightType() const
	{
		return m_Type;
	}

	ForceInline DIRECTIONAL_LIGHT_DATA GetDirectionalLight() const
	{
		assert(m_Type == LIGHT_TYPE_DIRECTIONAL);
		DIRECTIONAL_LIGHT_DATA Result = { };
		Result.Color = Color;
		XMStoreFloat3(&Result.Direction, GetOwner()->GetTransform()->GetForward());
		Result.Range = Range;
		Result.Intensity = Intensity;
		return Result;
	}

	ForceInline POINT_LIGHT_DATA GetPointLight() const
	{
		assert(m_Type == LIGHT_TYPE_POINT);
		POINT_LIGHT_DATA Result = { };
		Result.Color = Color;
		Result.Position = GetOwner()->GetTransform()->GetPosition();
		Result.Intensity = Intensity;
		Result.Range = Range;
		return Result;
	}

	ForceInline SPOTLIGHT_DATA GetSpotLight() const
	{
		assert(m_Type == LIGHT_TYPE_SPOT);
		SPOTLIGHT_DATA Result = { };
		Result.Color = Color;
		XMStoreFloat3(&Result.Direction, GetOwner()->GetTransform()->GetForward());
		Result.Position = GetOwner()->GetTransform()->GetPosition();
		Result.Range = Range;
		Result.SpotAngle = SpotAngle;
		Result.Intensity = Intensity;
		return Result;
	}
	
	XMFLOAT3 Color;
	float Range;
	float Intensity;
	float SpotAngle;


private:

	ELIGHT_TYPE m_Type;

};

