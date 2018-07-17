#pragma once

#include "RenderComponent.h"
#include "../Entity/Entity.h"
#include "KieckerMath.h"
#include "Graphics/Interfaces/GraphicsResources.h"


typedef enum ECAMERA_TYPE
{
	CAMERA_TYPE_ORTHO,
	CAMERA_TYPE_PROJECTION
} ECAMERA_TYPE;

class CameraComponent : public RenderComponent
{
	IMPLEMENT_BASE_CLASS(RenderComponent)
	IMPLEMENT_COMPONENT(CameraComponent)
public:

	CameraComponent();

	inline static CameraComponent* GetMainCamera()
	{
		return GMainCamera;
	}

	float Fov;
	float ViewWidth;
	float ViewHeight;
	float ZNear;
	float ZFar;
	ECAMERA_TYPE ProjectionType;
	
	inline void SetPriority(uint Priority)
	{
		m_Priority = Priority;
		if (m_Priority == 1)
		{
			GMainCamera = this;
		}
	}

	inline uint GetPriority() const
	{
		return m_Priority;
	}

	matrix GetProjection() const;
	matrix GetLookAt() const;

	void Tick(float DT) override final;

	void SubmitFeature(SceneRenderer* Renderer) final override;

private:

	static CameraComponent* GMainCamera;

	uint m_Priority;

};

