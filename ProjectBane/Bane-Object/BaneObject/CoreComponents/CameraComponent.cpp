#include "CameraComponent.h"
#include <Platform/System/Window.h>


CameraComponent* CameraComponent::GMainCamera = nullptr;

CameraComponent::CameraComponent() :
	Fov(60.0f),
	ViewWidth(Window::GetMainWindow()->AspectYX()),
	ViewHeight(1.0f),
	ZNear(0.01f),
	ZFar(1000.0f),
	ProjectionType(CAMERA_TYPE_PROJECTION)
{
}

matrix CameraComponent::GetProjection() const
{
	if (ProjectionType == CAMERA_TYPE_ORTHO)
	{
		return matrix();
		//return XMMatrixOrthographicLH(ViewWidth, ViewHeight, ZNear, ZFar);
	}
	else
	{
		float Aspect = Window::GetMainWindow()->AspectXY();
		return matProjection(Aspect, Fov, 0.01f, 1000.0f);
	}
}

matrix CameraComponent::GetLookAt() const
{
	static const float3 UpDirection = float3(0.0f, 1.0f, 0.0f);
	float3 Forward = GetOwner()->GetTransform()->GetForward();
	float3 Position = GetOwner()->GetTransform()->GetPosition();

	return matView(Position, Position + Forward, UpDirection);
}

void CameraComponent::Tick(float DT)
{
	UNUSED(DT);
}

void CameraComponent::GraphicsUpdate(RenderLoop& Loop)
{
	CAMERA_DATA CamData;
	CamData.Position = GetTransform()->GetPosition();
	CamData.Projection = GetProjection();
	CamData.View = GetLookAt();
	Loop.SetCamera(CamData);
}
