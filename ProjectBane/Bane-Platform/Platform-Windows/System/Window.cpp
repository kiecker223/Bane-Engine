#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../../Platform/System/Window.h"


#define CUSTOM_SIZE_EVENT 0x0401

Window* Window::GMainWindow = nullptr;

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


class OsWindowsHandle : public Window::WindowHandle {
public:
	static Window::WindowHandle* Create(LPCSTR className, const char* name, uint width, uint height, HINSTANCE instance);

	OsWindowsHandle(HWND);
	virtual ~OsWindowsHandle();
	virtual void Destroy();

protected:
	virtual void* GetHandle();
private:
	const HWND osHandle;
};

OsWindowsHandle::OsWindowsHandle(HWND handle)
	: osHandle(handle)
{

}

void OsWindowsHandle::Destroy() {
	delete this;
}

OsWindowsHandle::~OsWindowsHandle() {
	DestroyWindow(osHandle);
}

void* OsWindowsHandle::GetHandle() {
	return osHandle;
}

Window::WindowHandle* OsWindowsHandle::Create(LPCSTR className, const char* name, uint width, uint height, HINSTANCE instance) {

	auto windows = CreateWindowA(
		className, name, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr
	);

	return new OsWindowsHandle(windows);
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
	m_windowHandle->Destroy();
	UnregisterClassA("GAME-WINDOW-CLASS-NAME", GetModuleHandle(nullptr));
}

void Window::Open(const char* windowName, uint width, uint height, bool isFullscreen)
{
	HINSTANCE instance = GetModuleHandle(nullptr);

	WNDCLASSA wndClass = {};
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = instance;
	wndClass.lpszClassName = "EXPANSE-GAME-WINDOW-CLASS-NAME";
	wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	RegisterClassA(&wndClass);

	if (!isFullscreen)
	{
		m_windowHandle = OsWindowsHandle::Create(wndClass.lpszClassName, windowName, width, height, instance);
	}
	m_width = width;
	m_height = height;
}

bool Window::QuitRequested()
{
	bool hasQuit = false;

	MSG message = {};

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
		}
	}

	return hasQuit;
}
