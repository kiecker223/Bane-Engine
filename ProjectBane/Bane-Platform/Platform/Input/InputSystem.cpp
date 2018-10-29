#include "InputSystem.h"

InputSystem* InputSystem::GInstance = nullptr;

InputSystem::InputSystem(Window* pWindow) : Mouse(pWindow), MainWindow(pWindow)
{
	MainWindow->SetKeyboardDevice(&Keyboard);
}

void InputSystem::Initialize()
{

}

void InputSystem::Destroy()
{

}

void InputSystem::Update()
{
	Mouse.Update();
	Keyboard.Update();
}