#pragma once
#include "Common/Types.h"
#include <vcruntime_string.h>

typedef enum EKEY {
	// Letter
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
	KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
	KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
	KEY_Y, KEY_Z, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,
	KEY_7, KEY_8, KEY_9, KEY_0, KEY_TILDA,

	// Modifiers
	KEY_TAB,		KEY_CAPS,		KEY_LSHIFT,		KEY_LCONTROL,	KEY_LALT,	KEY_SPACE,
	KEY_MINUS,		KEY_PLUS,		KEY_BACKSPACE,
	KEY_LBRACKET,	KEY_RBRACKET,	KEY_SEMICOLON,	KEY_APOSTROPHE,	KEY_ENTER,
	KEY_COMMA,		KEY_PERIOD,		KEY_LSLASH,		KEY_RSLASH,
	KEY_RSHIFT,		KEY_RALT,		KEY_RCONTROL,

	KEY_UPARROW,	KEY_RIGHTARROW,	KEY_DOWNARROW,	KEY_LEFTARROW,
	KEY_NUM
} EKEY;

class InputKeyboardDevice
{
public:

	inline bool GetKeyDown(EKEY Key) const
	{
		return m_Keys[Key] && !m_PrevKeys[Key];
	}
	inline bool GetKeyUp(EKEY Key) const
	{
		return m_PrevKeys[Key] && !m_Keys[Key];
	}
	inline bool KeyHeld(EKEY Key) const
	{
		return m_PrevKeys[Key] && m_Keys[Key];
	}
	inline bool GetKey(EKEY Key) const 
	{
		return m_Keys[Key];
	}


	void Update();
	inline void Clear()
	{
		memset(m_Keys, 0, sizeof(m_Keys));
	}

private:

	bool m_Keys[EKEY::KEY_NUM];
	bool m_PrevKeys[EKEY::KEY_NUM];
};
