#include "Keyboard.h"

Keyboard::Keyboard()
{
	Initialize();
}

Keyboard::~Keyboard()
{

}

void Keyboard::Initialize()
{
	memset(m_keyStates, 0, KeyNum * sizeof(SHORT));
	memset(m_keyOldStates, 0, KeyNum * sizeof(SHORT));
}

void Keyboard::Input()
{
//現在のキーの状態を調べる
	memcpy(m_keyOldStates, m_keyStates, KeyNum * sizeof(SHORT));
	for (int i = 0;i < KeyNum;i++)
	{
		m_keyStates[i] = GetAsyncKeyState(i);
	}
}

bool Keyboard::isPressed(BYTE key) const
{
	if (m_keyOldStates[key] & 0x8000) return false;
	if (m_keyStates[key] & 0x8000) return true;
	return false;
}

bool Keyboard::isReleased(BYTE key) const
{
	if (m_keyStates[key] & 0x8000) return false;
	if (m_keyOldStates[key] & 0x8000) return true;
	return false;
}

bool Keyboard::isDown(BYTE key) const
{
	if (m_keyStates[key] & 0x8000) return true;
	return false;
}

bool Keyboard::isUp(BYTE key) const
{
	if (m_keyOldStates[key] & 0x8000) return false;
	return true;
}