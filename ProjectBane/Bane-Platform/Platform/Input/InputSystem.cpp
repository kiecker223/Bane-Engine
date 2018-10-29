#include "InputSystem.h"

InputSystem* InputSystem::GInstance = nullptr;

InputSystem::InputSystem(Window* pWindow) : Mouse(pWindow), MainWindow(pWindow)
{

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
}