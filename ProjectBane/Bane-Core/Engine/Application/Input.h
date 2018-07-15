#pragma once

#include "Common.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


class Input
{
public:
	Input();
	~Input();



private:

	IDirectInput8A*			m_Input;
	IDirectInputDevice8A*	m_Keyboard;
	IDirectInputDevice8A*	m_Mouse;
	IDirectInputDevice8A*	m_Controller;

	DIMOUSESTATE	m_MouseState;
	byte			m_Keys[256];
};

