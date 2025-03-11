#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "Math.h"

#pragma comment(lib,"xinput.lib")

class GamePad
{
public:
	GamePad(DWORD id);
	~GamePad();

	BYTE GetLeftTrigger() const { return m_leftTrigger; }
	BYTE GetRightTrigger() const { return m_rightTrigger; }
	XMFLOAT2 GetLeftStick() const { return m_leftStick; }
	XMFLOAT2 GetRightStick() const { return m_rightStick; }

	bool isPressed(int button) const;
	bool isReleased(int button) const;
	bool isDown(int button) const;
	bool isUp(int button) const;

	void Initialize();
	void Input();

private:
	bool ButtonIndex(int button, int& index) const;
	XMFLOAT2 CalcStickVector(float x, float y, float lenMin, float lenMax) const;

	static constexpr int ButtonNum = 14;
	static constexpr int ButtonList[ButtonNum] =
	{
		XINPUT_GAMEPAD_A,XINPUT_GAMEPAD_B,XINPUT_GAMEPAD_X,XINPUT_GAMEPAD_Y,
		XINPUT_GAMEPAD_DPAD_UP,XINPUT_GAMEPAD_DPAD_DOWN,XINPUT_GAMEPAD_DPAD_LEFT,XINPUT_GAMEPAD_DPAD_RIGHT,
		XINPUT_GAMEPAD_START,XINPUT_GAMEPAD_BACK,
		XINPUT_GAMEPAD_LEFT_THUMB,XINPUT_GAMEPAD_RIGHT_THUMB,
		XINPUT_GAMEPAD_LEFT_SHOULDER,XINPUT_GAMEPAD_RIGHT_SHOULDER
	};
	static constexpr float StickMinLength = 8000.0f;
	static constexpr float StickMaxLength = 30000.0f;

	DWORD m_padID;

	bool m_buttonStates[ButtonNum];
	bool m_buttonOldStates[ButtonNum];
	BYTE m_leftTrigger;
	BYTE m_rightTrigger;
	XMFLOAT2 m_leftStick;
	XMFLOAT2 m_rightStick;
};
