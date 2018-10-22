#pragma once

#include "BaneObject/Components/ComponentBase.h"
#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include "KieckerMath.h"
#include "BaneObject/Entity/Transform.h"
#include "Common.h"

class CameraMovementComponent : public Component
{
	IMPLEMENT_COMPONENT(CameraMovementComponent)

public:

	CameraMovementComponent();
	~CameraMovementComponent();

	uint8* InputState = nullptr;
	double Speed;

	void Start() override final
	{
		InputState = new uint8[256];
	}

	void Tick(float DT) override final
	{
		double Dt = static_cast<double>(DT);
		float RotationSpeed = 30.f;
		Transform* ST = GetTransform();
		GetKeyboardState(InputState);
		static const double3 Up(0.0, 1.0, 0.0);

		double3 Right;
		double3 Forward = ST->GetForward();
		Right = cross(Forward, Up);
		if (InputState[0x57] & 0x80)
		{
			ST->Translate((ST->GetForward() * Speed) * Dt);
		}
		if (InputState[0x53] & 0x80)
		{
			ST->Translate((-ST->GetForward() * Speed) * Dt);
		}
		if (InputState[0x41] & 0x80)
		{
			ST->Translate((Right * Speed) * Dt);
		}
		if (InputState[0x44] & 0x80)
		{
			ST->Translate((-Right * Speed) * Dt);
		}
		if (InputState[0x51] & 0x80)
		{
			ST->Rotate(fromDouble3(ST->GetRightVector()) * -RotationSpeed * DT);
		}
		if (InputState[0x52] & 0x80)
		{
			ST->Rotate(float3(0.0f, -RotationSpeed * DT, 0.0f) * fromDouble3(ST->GetUpVector()));
		}
		if (InputState[0x46] & 0x80)
		{
			ST->Rotate(float3(0.0f, RotationSpeed * DT, 0.0f) * fromDouble3(ST->GetUpVector()));
		}
		if (InputState[0x45] & 0x80)
		{
			ST->Rotate(fromDouble3(ST->GetRightVector()) * RotationSpeed * DT);
 		}
	}
};

