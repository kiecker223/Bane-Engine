#pragma once
#include "BaneMath.h"

class InputMouseDevice
{
	struct Impl;
public:

	InputMouseDevice(class Window* pWin);
	~InputMouseDevice();

	vec2 GetMouseDelta() const;
	vec2 GetMousePosition() const;
	float GetScrollWheel() const;
	bool GetButtonDown(uint32 Button) const;
	bool GetButtonUp(uint32 Button) const;
	bool GetDragging() const;

	void Update();

private:

	Impl* m_Impl;
};