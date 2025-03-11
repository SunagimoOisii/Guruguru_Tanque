#include "CameraActor.h"
#include "Game.h"
#include "Keyboard.h"
#include "Renderer.h"

CameraActor::CameraActor(class Game* game, Actor3D* target, bool fpvCamera)
	: Actor3D(game)
	, m_target(target)
	, m_fpvCamera(fpvCamera)
	, m_fpvOffset(ZeroVec3)
	, m_fpvMaxTilt(45.0f)
	, m_tpvBackOffset(1.0f)
	, m_tpvUpOffset(1.0f)
	, m_tpvFrontOffset(1.0f)
	, m_tpvAngle(0.0f)
	, m_tpvAngleSpeed(0.0f)
{
}

CameraActor::~CameraActor()
{
}

void CameraActor::SetFPVCameraParam(XMFLOAT3 offset, float maxTilt)
{
	m_fpvOffset  = offset;
	m_fpvMaxTilt = maxTilt;
}

void CameraActor::SetTPVCameraParam(float back, float up, float front, float angleSpeed)
{
	m_tpvBackOffset  = back;
	m_tpvUpOffset	 = up;
	m_tpvFrontOffset = front;
	m_tpvAngleSpeed  = angleSpeed;
}

void CameraActor::UpdateActor(float deltaTime)
{
	const Keyboard& keyB   = m_game->GetKeyboard();
	const GamePad& gamePad = m_game->GetGamePad(0);
	const std::unique_ptr<Renderer>& rend = m_game->GetRenderer();

	//TPVƒJƒƒ‰Žž‚Ì‰ñ“]ˆ—
	if (m_fpvCamera == false)
	{
		if (gamePad.isPressed(XINPUT_GAMEPAD_X))
		{
			m_tpvAngle = 0.0f;
		}
		else
		{
			int angle = 0;
			m_tpvAngle += m_tpvAngleSpeed * (float)angle * deltaTime;
			m_tpvAngle = (m_tpvAngle >= 360.0f) ? std::fmod(m_tpvAngle, 360.0f)
				: ((m_tpvAngle < 0.0f) ? 360.0f - std::fmod(std::fabs(m_tpvAngle), 360.0f) : m_tpvAngle);
		}
	}

	CalcWorldTransform();
	CamExtParam cam = (m_fpvCamera) ? CalcFPVCamera() : CalcTPVCamera();
	rend->SetCameraExtParam(cam);
}

void CameraActor::Draw()
{

}

/********************************************************************
ªpublic «private
********************************************************************/

CamExtParam CameraActor::CalcFPVCamera()
{
	//ƒ`ƒ‹ƒg“x‡‚¢‚ÌŒvŽZ
	const GamePad& gamePad = m_game->GetGamePad(0);
	float tilt = XMConvertToRadians(m_fpvMaxTilt * ((float)gamePad.GetRightTrigger() / 255.0f));

	XMFLOAT3 forward = m_target->GetForward();
	XMFLOAT3 right	 = m_target->GetRight();
	XMFLOAT3 up		 = m_target->GetUp();

	XMFLOAT3 forward_tilt = forward * std::cos(tilt) + up * std::sin(tilt);
	XMFLOAT3 up_tilt	  = forward * (-std::sin(tilt)) + up * std::cos(tilt);

	XMFLOAT3 pos = m_target->GetPos() + (forward_tilt * m_fpvOffset.x)
		+ (right * m_fpvOffset.y) + (up_tilt * m_fpvOffset.z);

	return CamExtParam(pos, pos + forward_tilt, up_tilt);
}

CamExtParam CameraActor::CalcTPVCamera()
{
	//ZŽ²‰ñ“]‚ÌŒvŽZ
	const GamePad& gamePad = m_game->GetGamePad(0);
	float angle = XMConvertToRadians(m_tpvAngle);

	XMFLOAT3 forward = m_target->GetForward();
	XMFLOAT3 right	 = m_target->GetRight();
	XMFLOAT3 up		 = m_target->GetUp();
	XMFLOAT3 pos	 = m_target->GetPos();

	XMFLOAT3 forward_angle = forward * std::cos(angle) + right * std::sin(angle);

	XMFLOAT3 cam	= pos + (-m_tpvBackOffset * forward_angle) + (m_tpvUpOffset * up);
	XMFLOAT3 target = pos + (m_tpvFrontOffset * forward_angle);

	return CamExtParam(cam, target, up);
}