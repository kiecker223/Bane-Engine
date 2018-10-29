#pragma once

#include "Common.h"
#include "../Input/InputMouseDevice.h"
#include "../Input/InputKeyboardDevice.h"

class Window
{
public:
	class WindowHandle {
	public:
		template <typename CAST> CAST GetNativeHandle() {
			return reinterpret_cast<CAST>(
				reinterpret_cast<intptr_t>(GetHandle()));
		}
		virtual void Destroy() { };
		virtual ~WindowHandle() { }

	protected:
		virtual void* GetHandle() { return nullptr; }
	};

	Window();
	Window(const char* windowName, uint32 width, uint32 height, bool isFullscreen = false);
	~Window();

	WindowHandle* GetHandle() const { return m_windowHandle; }
	inline uint32 GetWidth() const { return m_width; }
	inline uint32 GetHeight() const { return m_height; }
	inline float AspectXY() const { return (float)m_width / (float)m_height; }
	inline float AspectYX() const { return (float)m_height / (float)m_width; }

	void Open(const char* windowName, uint32 width, uint32 height, bool isFullscreen = false);
	inline void SetMouseDevice(InputMouseDevice* pDev)
	{
		m_Mouse = pDev;
	}

	inline void SetKeyboardDevice(InputKeyboardDevice* pDev)
	{
		m_Keyboard = pDev;
	}

	bool QuitRequested();

	static inline void SetMainWindow(Window* pWindow)
	{
		GMainWindow = pWindow;
	}

	static inline Window* GetMainWindow()
	{
		return GMainWindow;
	}

private:

	static Window* GMainWindow;
	uint32 m_width, m_height;
	WindowHandle* m_windowHandle;
	InputMouseDevice* m_Mouse;
	InputKeyboardDevice* m_Keyboard;
};

