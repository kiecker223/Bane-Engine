#pragma once

#include "Engine/Engine.h"


class CameraMovementComponent : public Component
{
	IMPLEMENT_COMPONENT(CameraMovementComponent)

public:

	CameraMovementComponent();
	~CameraMovementComponent();

	uint8* InputState = nullptr;

	void Start() override final
	{
		InputState = new uint8[256];
	}

	void Tick(double Dt) override final
	{
		GetKeyboardState(InputState);

		if (InputState[0x57] & 0x80)
		{
			GetOwner()->GetTransform()->Translate(GetOwner()->GetTransform()->GetForward());
		}
		if (InputState[0x53] & 0x80)
		{
			GetOwner()->GetTransform()->Translate(-GetOwner()->GetTransform()->GetForward());
		}
		if (InputState[0x41] & 0x80)
		{
			XMVECTOR Right;
			XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
			XMFLOAT3 Forward;
			XMStoreFloat3(&Forward, GetOwner()->GetTransform()->GetForward());
			Right = XMVector3Cross(XMLoadFloat3(&Forward), XMLoadFloat3(&Up));
			GetOwner()->GetTransform()->Translate(Right * 0.3f);
		}
		if (InputState[0x44] & 0x80)
		{
			XMVECTOR Right;
			XMFLOAT3 Up(0.0f, 1.0f, 0.0f);
			XMFLOAT3 Forward;
			XMStoreFloat3(&Forward, GetOwner()->GetTransform()->GetForward());
			Right = XMVector3Cross(XMLoadFloat3(&Forward), XMLoadFloat3(&Up));
			GetOwner()->GetTransform()->Translate(-Right * 0.3f);
		}
		if (InputState[0x51] & 0x80)
		{
			//GetOwner()->GetTransform()->Rotate(0.0f, 0.005f, 0.0f);
			GetOwner()->GetTransform()->Rotate(0.5f, 0.0f, 0.0f);
		}
		if (InputState[0x52] & 0x80)
		{
			GetOwner()->GetTransform()->Rotate(0.0f, 0.5f, 0.0f);
		}
		if (InputState[0x46] & 0x80)
		{
			GetOwner()->GetTransform()->Rotate(0.0f, -0.5f, 0.0f);
		}
		if (InputState[0x45] & 0x80)
		{
			//GetOwner()->GetTransform()->Rotate(0.0f, 0.005f, 0.0f);
			GetOwner()->GetTransform()->Rotate(-0.5f, 0.0f, 0.0f);
		}

	}
};

