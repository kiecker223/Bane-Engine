#pragma once

#include "../Core/Common.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


class Window
{
public:
	Window();
	Window(const char* windowName, uint width, uint height, bool isFullscreen = false);
	~Window();

	inline HWND GetHwnd() const		{ return m_windowHandle; }
	inline uint GetWidth() const	{ return m_width;		 }
	inline uint GetHeight() const	{ return m_height;		 }

	void Open(const char* windowName, uint width, uint height, bool isFullscreen = false);
	 
	bool QuitRequested();

private:

	static Window* sm_WindowInstance;

	uint m_width, m_height;
	HWND m_windowHandle;
};

