#pragma once

#include "BaneObject/Components/ComponentBase.h"
#include <iostream>
#include <windows.h>
#include <WinUser.h>
#include <Platform/Input/InputSystem.h>
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
	}

	void Tick(float DT) override final
	{
		double Dt = static_cast<double>(DT);
		float RotationSpeed = 1.0f;
		Transform* ST = GetTransform();
		static const double3 Up(0.0, 1.0, 0.0);
		float2 MouseDelta = GetInput()->Mouse.GetMouseDelta();
		double3 Forward = ST->GetForward();
		double3 Right = ST->GetRightVector();
		if (GetInput()->Keyboard.GetKey(KEY_W))
		{
			ST->Translate((ST->GetForward() * Speed) * Dt);
		}
		if (GetInput()->Keyboard.GetKey(KEY_S))
		{
			ST->Translate((-ST->GetForward() * Speed) * Dt);
		}
		if (GetInput()->Keyboard.GetKey(KEY_D))
		{
			ST->Translate((Right * Speed) * Dt);
		}
		if (GetInput()->Keyboard.GetKey(KEY_A))
		{
			ST->Translate((-Right * Speed) * Dt);
		}
		if (GetInput()->Mouse.GetScrollWheel() > 0.f)
		{
			Speed *= 10.0;
		}
		if (GetInput()->Mouse.GetScrollWheel() < 0.f)
		{
			Speed /= 10.0;
		}
		if (abs(MouseDelta.y) > 0.f)
 		{
			ST->GetRotation() *= Quaternion::FromAxisAngle(Right, MouseDelta.y * RotationSpeed * DT);
 			ST->GetRotation().Normalize();
		}
 		if (abs(MouseDelta.x) > 0.f)
 		{
			ST->GetRotation() *= Quaternion::FromAxisAngle(float3(0.f, 1.f, 0.f), MouseDelta.x * RotationSpeed * DT);
 			ST->GetRotation().Normalize();
		}
	}
};

