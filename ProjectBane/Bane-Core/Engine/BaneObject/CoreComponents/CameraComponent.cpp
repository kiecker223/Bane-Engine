#include "CameraComponent.h"
#include "Application/Window.h"
#include "Application/Application.h"


CameraComponent* CameraComponent::GMainCamera = nullptr;

CameraComponent::CameraComponent() :
	Fov(60.0f),
	ViewWidth(((float)GetApplicationInstance()->GetWindow()->GetWidth() / (float)GetApplicationInstance()->GetWindow()->GetHeight())),
	ViewHeight(1.0f),
	ZNear(0.01f),
	ZFar(1000.0f),
	ProjectionType(CAMERA_TYPE_PROJECTION)
{
}

XMMATRIX CameraComponent::GetProjection() const
{
	if (ProjectionType == CAMERA_TYPE_ORTHO)
	{
		return XMMatrixOrthographicLH(ViewWidth, ViewHeight, ZNear, ZFar);
	}
	else
	{
		return XMMatrixTranspose(XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), ViewWidth, ZNear, ZFar));
	}
	return XMMATRIX();
}

XMMATRIX CameraComponent::GetLookAt() const
{
	static const XMFLOAT3 UpDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR Forward = GetOwner()->GetTransform()->GetForward();
	XMVECTOR LookForward = XMLoadFloat3(&GetOwner()->GetTransform()->GetPosition()) + Forward;
	//XMVECTOR UpVector = XMVector3Cross(XMLoadFloat3(&UpDirection), Forward);

	return XMMatrixTranspose(XMMatrixLookAtLH(
		XMLoadFloat3(&GetOwner()->GetTransform()->GetPosition()),
		LookForward,
		XMLoadFloat3(&UpDirection)
	));
}

void CameraComponent::Tick(double DT)
{
}

void CameraComponent::SubmitFeature(SceneRenderer* Renderer)
{
	Renderer->AddCamera(this);
}
