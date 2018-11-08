#pragma once

#include "RenderComponent.h"
#include "../Entity/Entity.h"
#include "KieckerMath.h"
#include <Graphics/Data/RenderLoop.h>


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

	float Fov;
	float ViewWidth;
	float ViewHeight;
	float ZNear;
	float ZFar;
	ECAMERA_TYPE ProjectionType;
	
	float4x4 GetProjection() const;
	float4x4 GetLookAt() const;

	void Start() override final;
	void Tick(float DT) override final;

	void GraphicsUpdate(RenderLoop& Loop) final override;

private:


};

