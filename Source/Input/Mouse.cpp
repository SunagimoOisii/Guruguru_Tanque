#include "Mouse.h"
#include "Math.h"

Mouse::Mouse()
	:m_hwnd(nullptr)
	,m_pos(ZeroVec2)
{
	Initialize(nullptr);
}

Mouse::~Mouse()
{
}

void Mouse::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;
	memset(m_keyStates, 0, KeyNum * sizeof(SHORT));
	memset(m_keyOldStates, 0, KeyNum * sizeof(SHORT));
}

void Mouse::Input()
{
	memcpy(m_keyOldStates, m_keyStates, KeyNum * sizeof(SHORT));

	for (int i = 0;i < KeyNum;i++)
	{
		m_keyStates[i] = GetAsyncKeyState(VK_Keys[i]);
	}

	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(m_hwnd, &pos);
	m_pos = XMFLOAT2(static_cast<float>(pos.x), static_cast<float>(pos.y));
}

bool Mouse::KeyIndex(BYTE key, int& index) const
{
	for (index = 0;index < KeyNum;index++)
	{
		if (key == VK_Keys[index]) break;
	}
	if (index >= KeyNum) return false;

	return true;
}

bool Mouse::isPressed(BYTE key) const
{
	int index = 0;
	if (!KeyIndex(key, index)) return false;

	if (m_keyOldStates[index] & 0x8000) return false;
	if (m_keyStates[index] & 0x8000) return true;
	return false;
}

bool Mouse::isReleased(BYTE key) const
{
	int index = 0;
	if (!KeyIndex(key, index)) return false;

	if (m_keyStates[index] & 0x8000) return false;
	if (m_keyOldStates[index] & 0x8000) return true;
	return false;
}

bool Mouse::isDown(BYTE key) const
{
	int index = 0;
	if (!KeyIndex(key, index)) return false;

	if (m_keyStates[index] & 0x8000) return true;
	return false;
}

bool Mouse::isUp(BYTE key) const
{
	int index = 0;
	if (!KeyIndex(key, index)) return false;

	if (m_keyStates[index] & 0x8000) return false;
	return true;
}