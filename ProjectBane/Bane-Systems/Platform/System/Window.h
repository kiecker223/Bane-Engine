#pragma once

#include "Common.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


class Window
{
public:
	Window();
	Window(const char* windowName, uint width, uint height, bool isFullscreen = false);
	~Window();

	inline HWND GetHwnd() const { return m_windowHandle; }
	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }
	inline float AspectXY() const { return (float)m_width / (float)m_height; }
	inline float AspectYX() const { return (float)m_height / (float)m_width; }

	void Open(const char* windowName, uint width, uint height, bool isFullscreen = false);

	bool QuitRequested();

private:

	uint m_width, m_height;
	HWND m_windowHandle;
};

