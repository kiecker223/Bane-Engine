#pragma once
#include <KieckerMath.h>

class InputMouseDevice
{
	struct Impl;
public:

	InputMouseDevice(class Window* pWin);
	~InputMouseDevice();

	float2 GetMouseDelta() const;
	float2 GetMousePosition() const;
	float GetScrollWheel() const;
	bool GetButtonDown(uint32 Button) const;
	bool GetButtonUp(uint32 Button) const;
	bool GetDragging() const;

	void Update();

private:

	Impl* m_Impl;
};