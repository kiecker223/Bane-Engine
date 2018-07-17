#pragma once

#include "RenderComponent.h"
#include "BaneObject/Entity/Entity.h"

typedef struct DIRECTIONAL_LIGHT_DATA {
	float3 Direction;
	float Range;
	float3 Color;
	float Intensity;
} DIRECTIONAL_LIGHT_DATA;

typedef struct POINT_LIGHT_DATA {
	float3 Position;
	float Range;
	float3 Color;
	float Intensity;
} POINT_LIGHT_DATA;

typedef struct SPOTLIGHT_DATA {
	float3 Position;
	float Range;
	float3 Direction;
	float Intensity;
	float3 Color;
	float SpotAngle;
} SPOTLIGHT_DATA;

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

	inline ELIGHT_TYPE GetLightType() const
	{
		return m_Type;
	}

	inline DIRECTIONAL_LIGHT_DATA GetDirectionalLight() const;

	inline POINT_LIGHT_DATA GetPointLight() const;

	inline SPOTLIGHT_DATA GetSpotLight() const;
	
	float3 Color;
	float Range;
	float Intensity;
	float SpotAngle;

private:

	ELIGHT_TYPE m_Type;

};

