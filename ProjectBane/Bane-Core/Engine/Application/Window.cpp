#include "Window.h"
#include "Graphics/IO/ShaderCache.h"


Window* Window::sm_WindowInstance;

#define CUSTOM_SIZE_EVENT 0x0401

LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			PostQuitMessage(0);
			break;
		case WM_SIZING:
			SendMessageA(window, CUSTOM_SIZE_EVENT, wparam, lparam);
			break;
	}
	return DefWindowProcA(window, message, wparam, lparam);
}


Window::Window()
{
}

Window::Window(const char *windowName, uint width, uint height, bool isFullscreen) 
{
	Open(windowName, width, height, isFullscreen);
}

Window::~Window()
{
	DestroyWindow(m_windowHandle);
	UnregisterClassA("EXPANSE-GAME-WINDOW-CLASS-NAME", GetModuleHandle(nullptr));
}

void Window::Open(const char* windowName, uint width, uint height, bool isFullscreen)
{
	HINSTANCE instance = GetModuleHandle(nullptr);

	WNDCLASSA wndClass = { };
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = instance;
	wndClass.lpszClassName = "EXPANSE-GAME-WINDOW-CLASS-NAME";
	wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	RegisterClassA(&wndClass);

	if (!isFullscreen)
	{
		m_windowHandle = CreateWindowA(
			wndClass.lpszClassName, windowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr
		);
	}
	m_width = width;
	m_height = height;
}

bool Window::QuitRequested()
{
	bool hasQuit = false;

	MSG message = { };
	
	while (PeekMessageA(&message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
		switch (message.message)
		{
		case WM_QUIT:
			hasQuit = true;
			break;
		case CUSTOM_SIZE_EVENT:
		{
			RECT* newSizes = (RECT*)message.lParam;
			m_width = newSizes->right - newSizes->left;
			m_height = newSizes->top - newSizes->bottom;
		} break;
		case WM_KEYDOWN:
		{
#ifdef _DEBUG
			auto Key = message.wParam;
			if (Key == 0x50)
			{
				GetShaderCache()->TransitionToReloadState();
			}
#endif
		} break;
		}
	}

	return hasQuit;
}
