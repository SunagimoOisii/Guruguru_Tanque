#pragma once

#include "Actor3D.h"
#include "Camera.h"

class CameraActor : public Actor3D
{
public:
	CameraActor(class Game* game, Actor3D* target, bool fpvCamera = true);
	~CameraActor();

	void SetFPVCameraParam(XMFLOAT3 offset, float maxTilt);
	void SetTPVCameraParam(float back, float up, float front, float angleSpeed);

	void UpdateActor(float deltaTime) override;
	void Draw() override;

private:
	CamExtParam CalcFPVCamera();
	CamExtParam CalcTPVCamera();

	Actor3D* m_target;

	//カメラ(FPV)
	bool	 m_fpvCamera;
	XMFLOAT3 m_fpvOffset;
	float	 m_fpvMaxTilt;
	//カメラ(TPV)
	float m_tpvBackOffset;
	float m_tpvUpOffset;
	float m_tpvFrontOffset;
	float m_tpvAngle;
	float m_tpvAngleSpeed;
};
