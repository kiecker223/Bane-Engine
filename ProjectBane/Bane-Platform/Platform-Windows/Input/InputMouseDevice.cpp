#include "../../Platform/Input/InputMouseDevice.h"
#include "../../Platform/System/Window.h"
#include <Windows.h>
#include <dinput.h>

struct InputMouseDevice::Impl
{
	Impl(Window* pWin)
	{
		UNUSED(pWin);
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

		BANE_CHECK(
			SUCCEEDED(
				DirectInput->CreateDevice(
					GUID_SysKeyboard,
					&Keyboard,
					nullptr
				)
			)
		);

		Keyboard->SetDataFormat(&c_dfDIKeyboard);
	}

	IDirectInput8* DirectInput;
	IDirectInputDevice8* Mouse;
	IDirectInputDevice8* Keyboard;
	float2 MouseDelta;
	float2 MousePosition;
	float2 LastMousePosition;
	bool bButtons[5];
	bool bLastButtons[5];
	bool bDragging;

	void Update()
	{
		DIMOUSESTATE2 MouseState = { };
		Mouse->Acquire();
		Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &MouseState);
		MousePosition = LastMousePosition;
		MousePosition = float2(static_cast<float>(MouseState.lX), static_cast<float>(MouseState.lY));
		MouseDelta = MousePosition - LastMousePosition;
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
