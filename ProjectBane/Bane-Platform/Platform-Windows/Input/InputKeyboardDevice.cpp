#include "../../Platform/Input/InputKeyboardDevice.h"
#include <Math/SIMDFunctions.h>
#include <Windows.h>


static uint8 WindowsKeyState[256];

void InputKeyboardDevice::Update()
{
	memcpy(m_PrevKeys, m_Keys, sizeof(m_Keys));

	GetKeyboardState(WindowsKeyState);

	for (uint32 i = 0x30, b = 0; i < 0x3A; i++, b++)
	{
		m_Keys[EKEY::KEY_0 + b] = WindowsKeyState[i] & 0x80;
	}

	for (uint32 i = 0x41, b = 0; i < 0x5B; i++, b++)
	{
		m_Keys[EKEY::KEY_A + b] = WindowsKeyState[i] & 0x80;
	}

	m_Keys[KEY_TILDA]		= WindowsKeyState[VK_OEM_3] & 0x80;
	m_Keys[KEY_TAB]			= WindowsKeyState[VK_TAB] & 0x80;
	m_Keys[KEY_CAPS]		= WindowsKeyState[VK_CAPITAL] & 0x80;
	m_Keys[KEY_LSHIFT]		= WindowsKeyState[VK_LSHIFT] & 0x80;
	m_Keys[KEY_LCONTROL]	= WindowsKeyState[VK_LCONTROL] & 0x80;
	m_Keys[KEY_LALT]		= WindowsKeyState[VK_MENU] & 0x80;
	m_Keys[KEY_SPACE]		= WindowsKeyState[VK_SPACE] & 0x80;
	m_Keys[KEY_MINUS]		= WindowsKeyState[VK_OEM_MINUS] & 0x80;
	m_Keys[KEY_PLUS]		= WindowsKeyState[VK_OEM_PLUS] & 0x80;
	m_Keys[KEY_BACKSPACE]	= WindowsKeyState[VK_BACK] & 0x80;
	m_Keys[KEY_LBRACKET]	= WindowsKeyState[VK_OEM_4] & 0x80;
	m_Keys[KEY_RBRACKET]	= WindowsKeyState[VK_OEM_6] & 0x80;
	m_Keys[KEY_SEMICOLON]	= WindowsKeyState[VK_OEM_1] & 0x80;
	m_Keys[KEY_APOSTROPHE]	= WindowsKeyState[VK_OEM_7] & 0x80;
	m_Keys[KEY_ENTER]		= WindowsKeyState[VK_RETURN] & 0x80;
	m_Keys[KEY_COMMA]		= WindowsKeyState[VK_OEM_COMMA] & 0x80;
	m_Keys[KEY_PERIOD]		= WindowsKeyState[VK_OEM_PERIOD] & 0x80;
	m_Keys[KEY_LSLASH]		= WindowsKeyState[VK_OEM_2] & 0x80;
	m_Keys[KEY_RSLASH]		= WindowsKeyState[VK_OEM_5] & 0x80;
	m_Keys[KEY_RSHIFT]		= WindowsKeyState[VK_RSHIFT] & 0x80;
	m_Keys[KEY_RCONTROL]	= WindowsKeyState[VK_RCONTROL] & 0x80;
	m_Keys[KEY_UPARROW]		= WindowsKeyState[VK_UP] & 0x80;
	m_Keys[KEY_RIGHTARROW]	= WindowsKeyState[VK_RIGHT] & 0x80;
	m_Keys[KEY_DOWNARROW]	= WindowsKeyState[VK_DOWN] & 0x80;
	m_Keys[KEY_LEFTARROW]	= WindowsKeyState[VK_LEFT] & 0x80;
}
