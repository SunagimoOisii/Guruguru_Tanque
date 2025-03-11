#pragma once

#include <Windows.h>

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	bool isPressed(BYTE key) const;
	bool isReleased(BYTE key) const;
	bool isDown(BYTE key) const;
	bool isUp(BYTE key) const;

	void Initialize();
	void Input();

private:
	static constexpr int KeyNum = 256;

	SHORT m_keyStates[KeyNum];
	SHORT m_keyOldStates[KeyNum];
};
