#pragma once

#include "Common.h"


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
<<<<<<< HEAD
		virtual void* GetHandle() { return nullptr; }
=======
		virtual void* GetHandle() = 0;
>>>>>>> 63af680d424978a61b19e93dd0e77b490f0a6b80
	};

	Window();
	Window(const char* windowName, uint width, uint height, bool isFullscreen = false);
	~Window();

	WindowHandle* GetHandle() const { return m_windowHandle; }
	inline uint GetWidth() const { return m_width; }
	inline uint GetHeight() const { return m_height; }
	inline float AspectXY() const { return (float)m_width / (float)m_height; }
	inline float AspectYX() const { return (float)m_height / (float)m_width; }

	void Open(const char* windowName, uint width, uint height, bool isFullscreen = false);

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
	uint m_width, m_height;
	WindowHandle* m_windowHandle;
};

