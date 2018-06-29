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

	void Tick(float Dt) override final
	{
		GetKeyboardState(InputState);
		static const float3 Up(0.0f, 1.0f, 0.0f);

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
			float3 Right;
			float3 Forward = GetOwner()->GetTransform()->GetForward();
			Right = cross(Forward, Up);
			GetOwner()->GetTransform()->Translate(Right * 0.3f);
		}
		if (InputState[0x44] & 0x80)
		{
			float3 Right;
			float3 Forward = GetOwner()->GetTransform()->GetForward();
			Right = cross(Forward, Up);
			GetOwner()->GetTransform()->Translate(-Right * 0.3f);
		}
		if (InputState[0x51] & 0x80)
		{
			//GetOwner()->GetTransform()->Rotate(0.0f, 0.005f, 0.0f);
			GetOwner()->GetTransform()->Rotate(-5.f, 0.0f, 0.0f);
		}
		if (InputState[0x52] & 0x80)
		{
			GetOwner()->GetTransform()->Rotate(0.0f, 5.f, 0.0f);
		}
		if (InputState[0x46] & 0x80)
		{
			GetOwner()->GetTransform()->Rotate(0.0f, 5.f, 0.0f);
		}
		if (InputState[0x45] & 0x80)
		{
			//GetOwner()->GetTransform()->Rotate(0.0f, 0.005f, 0.0f);
			GetOwner()->GetTransform()->Rotate(5.f, 0.0f, 0.0f);
		}

	}
};

