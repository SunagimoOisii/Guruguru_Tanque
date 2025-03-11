#include "GamePad.h"

GamePad::GamePad(DWORD id)
	:m_padID(id)
	, m_leftTrigger(0)
	, m_rightTrigger(0)
	, m_leftStick(ZeroVec2)
	, m_rightStick(ZeroVec2)
{
	Initialize();
}

GamePad::~GamePad()
{
}

void GamePad::Initialize()
{
	memset(m_buttonStates, false, sizeof(bool) * ButtonNum);
	memset(m_buttonOldStates, false, sizeof(bool) * ButtonNum);

	m_leftTrigger  = 0;
	m_rightTrigger = 0;
	m_leftStick  = ZeroVec2;
	m_rightStick = ZeroVec2;
}

void GamePad::Input()
{
	memcpy(m_buttonOldStates, m_buttonStates, sizeof(bool) * ButtonNum);

	XINPUT_STATE state{};
	DWORD result = XInputGetState(m_padID, &state);
	if (result != ERROR_SUCCESS)
	{
		Initialize();
	}
	else
	{
		for (int i = 0;i < ButtonNum;i++)
		{
			m_buttonStates[i] = state.Gamepad.wButtons & ButtonList[i];
		}
		m_leftTrigger = state.Gamepad.bLeftTrigger;
		m_rightTrigger = state.Gamepad.bRightTrigger;
	}

	m_leftStick = CalcStickVector(static_cast<float>(state.Gamepad.sThumbLX),
		-1.0f * static_cast<float>(state.Gamepad.sThumbLY), StickMinLength, StickMaxLength);
	m_rightStick = CalcStickVector(static_cast<float>(state.Gamepad.sThumbRX),
		-1.0f * static_cast<float>(state.Gamepad.sThumbRY), StickMinLength, StickMaxLength);
}

bool GamePad::ButtonIndex(int button, int& index) const
{
	for (index = 0;index < ButtonNum;index++)
	{
		if (button == ButtonList[index]) return true;
	}
	return false;
}

bool GamePad::isPressed(int button) const
{
	int index = 0;
	if (!ButtonIndex(button, index)) return false;
	return m_buttonStates[index] && !m_buttonOldStates[index];
}

bool GamePad::isReleased(int button) const
{
	int index = 0;
	if (!ButtonIndex(button, index)) return false;
	return !m_buttonStates[index] && m_buttonOldStates[index];
}

bool GamePad::isDown(int button) const
{
	int index = 0;
	if (!ButtonIndex(button, index)) return false;
	return m_buttonStates[index];
}

bool GamePad::isUp(int button) const
{
	int index = 0;
	if (!ButtonIndex(button, index)) return false;
	return !m_buttonStates[index];
}

XMFLOAT2 GamePad::CalcStickVector(float x, float y, float lenMin, float lenMax) const
{
	XMFLOAT2 s(x, y);
	
	float len = Length(s);
	if (len <= lenMin) return ZeroVec2;

	len = (len >= lenMax) ? lenMax : len;
	return Normalize(s) * ((len - lenMin) / (lenMax - lenMin));
}