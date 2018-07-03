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
		Transform* ST = GetTransform();
		GetKeyboardState(InputState);
		static const float3 Up(0.0f, 1.0f, 0.0f);

		float3 Right;
		float3 Forward = ST->GetForward();
		Right = cross(Forward, Up);

		if (InputState[0x57] & 0x80)
		{
			ST->Translate(ST->GetForward());
		}
		if (InputState[0x53] & 0x80)
		{
			ST->Translate(-ST->GetForward());
		}
		if (InputState[0x41] & 0x80)
		{
			ST->Translate(Right * 0.3f);
		}
		if (InputState[0x44] & 0x80)
		{
			ST->Translate(-Right * 0.3f);
		}
		if (InputState[0x51] & 0x80)
		{
			ST->Rotate(-5.f, 0.0f, 0.0f);
		}
		if (InputState[0x52] & 0x80)
		{
			ST->Rotate(0.0f, -5.f, 0.0f);
		}
		if (InputState[0x46] & 0x80)
		{
			ST->Rotate(0.0f, 5.f, 0.0f);
		}
		if (InputState[0x45] & 0x80)
		{
			ST->Rotate(5.f, 0.0f, 0.0f);
		}

	}
};

