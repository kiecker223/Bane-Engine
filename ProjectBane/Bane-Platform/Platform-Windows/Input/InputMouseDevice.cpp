#include "../../Platform/Input/InputMouseDevice.h"
#include "../../Platform/System/Window.h"
#include <Windows.h>
#include <dinput.h>

struct InputMouseDevice::Impl
{
	Impl(Window* pWin)
	{
		WindowSize = float2(static_cast<float>(pWin->GetWidth()), static_cast<float>(pWin->GetHeight()));
		POINT Cursor;
		GetCursorPos(&Cursor);
		MousePosition = float2(static_cast<float>(Cursor.x), static_cast<float>(Cursor.y));

		BANE_CHECK(
			SUCCEEDED(
				DirectInput8Create(
					GetModuleHandle(nullptr),
					DIRECTINPUT_VERSION,
					IID_IDirectInput8,
					reinterpret_cast<void**>(&DirectInput),
					nullptr
				)
			)
		);

		BANE_CHECK(
			SUCCEEDED(
				DirectInput->CreateDevice(
					GUID_SysMouse,
					&Mouse,
					nullptr
				)
			)
		);

		Mouse->SetDataFormat(&c_dfDIMouse2);

	}
	IDirectInput8* DirectInput;
	IDirectInputDevice8* Mouse;
	float2 MouseDelta;
	float2 MousePosition;
	float2 LastMousePosition;
	float2 WindowSize;
	float MouseScroll;
	bool bButtons[5];
	bool bLastButtons[5];
	bool bDragging;

	void Update()
	{
		DIMOUSESTATE2 MouseState = { };
		Mouse->Acquire();
		Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &MouseState);
		LastMousePosition = MousePosition;
		MouseDelta = float2(static_cast<float>(MouseState.lX), static_cast<float>(MouseState.lY)) * (WindowSize.y / WindowSize.x);
		MousePosition += MouseDelta;
		MouseScroll = static_cast<float>(MouseState.lZ);
	}
};

InputMouseDevice::InputMouseDevice(Window* pWin)
{
	m_Impl = new Impl(pWin);
	pWin->SetMouseDevice(this);
}

InputMouseDevice::~InputMouseDevice()
{
	delete m_Impl;
}

float2 InputMouseDevice::GetMouseDelta() const
{
	return m_Impl->MouseDelta;
}

float2 InputMouseDevice::GetMousePosition() const
{
	return m_Impl->MousePosition;
}

float InputMouseDevice::GetScrollWheel() const
{
	return m_Impl->MouseScroll;
}

bool InputMouseDevice::GetButtonDown(uint32 Button) const
{
	return m_Impl->bButtons[Button];
}

bool InputMouseDevice::GetButtonUp(uint32 Button) const
{
	return m_Impl->bLastButtons[Button];
}

bool InputMouseDevice::GetDragging() const
{
	return m_Impl->bDragging;
}

void InputMouseDevice::Update()
{
	m_Impl->Update();
}
