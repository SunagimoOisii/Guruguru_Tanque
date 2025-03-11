#pragma once

#include "Math.h"
#include <Windows.h>

class Mouse
{
public:
	Mouse();
	~Mouse();

	XMFLOAT2 GetPos() const { return m_pos; }

	bool isPressed(BYTE key) const;
	bool isReleased(BYTE key) const;
	bool isDown(BYTE key) const;
	bool isUp(BYTE key) const;

	void Initialize(HWND hwnd);
	void Input();

private:
	bool KeyIndex(BYTE key, int& index) const;

	HWND m_hwnd;
	XMFLOAT2 m_pos;

	static constexpr int KeyNum = 3;
	static constexpr BYTE VK_Keys[KeyNum] = { VK_LBUTTON,VK_RBUTTON,VK_MBUTTON };

	SHORT m_keyStates[KeyNum];
	SHORT m_keyOldStates[KeyNum];
};
