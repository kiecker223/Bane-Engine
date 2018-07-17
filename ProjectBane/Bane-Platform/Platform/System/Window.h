#pragma once

#include "Common.h"


class Window
{
public:
	class WindowsHandle {
	public:
		template <typename CAST> CAST GetNativeHandle() {
			return reinterpret_cast<CAST>(
				reinterpret_cast<intptr_t>(GetHandle()));
		}
		virtual void Destroy() { };
		virtual ~WindowsHandle() { }

	protected:
		virtual void* GetHandle();
	};

	Window();
	Window(const char* windowName, uint width, uint height, bool isFullscreen = false);
	~Window();

	WindowsHandle* GetHandle() const { return m_windowHandle; }
	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }
	inline float AspectXY() const { return (float)m_width / (float)m_height; }
	inline float AspectYX() const { return (float)m_height / (float)m_width; }

	void Open(const char* windowName, uint width, uint height, bool isFullscreen = false);

	bool QuitRequested();

private:

	uint m_width, m_height;
	WindowsHandle* m_windowHandle;
};

