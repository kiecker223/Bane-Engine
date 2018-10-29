#pragma once
#include "../System/Window.h"
#include "InputMouseDevice.h"
#include "InputKeyboardDevice.h"

class InputSystem
{
public:

	InputSystem(Window* pWindow);

	void Initialize();
	void Destroy();
	void Update();

	Window* MainWindow;
	InputMouseDevice	Mouse;
	InputKeyboardDevice Keyboard;
	static InputSystem* GInstance;
};

inline void InitializeInput(Window* pWindow)
{
	InputSystem::GInstance = new InputSystem(pWindow);
	InputSystem::GInstance->Initialize();
}

inline void DestroyInput()
{
	InputSystem::GInstance->Destroy();
	delete InputSystem::GInstance;
}

inline void UpdateInput()
{
	InputSystem::GInstance->Update();
}

inline InputSystem* GetInput()
{
	return InputSystem::GInstance;
}
