#pragma once

#include "SpriteActor.h"
#include "FileMeshActor.h"
#include "CameraActor.h"

class PlayerActor : public FileMeshActor
{
public:
	PlayerActor(class Game* game, int shaderClassIndex,
		const wchar_t* filePath, int ID, float scaleLong, float scaleWidth,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3);
	~PlayerActor();

	//bool IsReticleMode() const { return m_reticleMode; }
	Shape::Circle GetBoundingCircle() const
	{
		float radius = (m_scaleLong > m_scaleWidth) ? m_scaleLong : m_scaleWidth;
		return Shape::Circle(XMFLOAT2(m_pos.x, m_pos.y), radius);
	}

	float getRadius() const
	{
		return (m_scaleLong > m_scaleWidth) ? m_scaleLong : m_scaleWidth;
	}


	Shape::RotateRect GetBoundingRect();
	//XMFLOAT2 GetReticlePos() const { return m_reticle->GetPos(); }

	void UpdateActor(float deltaTime) override;

	void Draw() override;

	float getGurugurRight() {return m_RGuruguruAngle; }
	float getGurugurLeft() { return m_LGuruguruAngle; }
	float getPrevGurugurRight() { return m_prevFrameRightGuruguruAngle; }
	float getPrevGurugurLeft() { return m_prevFrameLeftGuruguruAngle; }
	float getGurugurRightPower() { return m_RGuruguruPower; }
	float getGurugurLeftPower() { return m_moveForce; }

	int getID() { return m_ID; }

	int getHP() { return m_hp; }

	void setShootingStates(float shootDelay, float bulletSpeed, bool isFullAuto)
	{
		m_shootDelay = shootDelay;
		m_bulletSpeed = bulletSpeed;
		m_isFullAuto = isFullAuto;
	}

	void setMoveStates(float moveSpeed, float angleSpeed, float moveForceVel, float moveForceMax);

private:
	static const float PermissibleHeight;

	//std::unique_ptr<CameraActor> m_camera;

	int m_ID;
	float m_scaleLong;
	float m_scaleWidth;

	//std::unique_ptr<SpriteActor> m_reticle;
	//bool  m_reticleMode;
	//float m_reticleSpeed;
	//float m_reticleRadius;

	float m_prevFrameRightGuruguruAngle;
	float m_prevFrameLeftGuruguruAngle;

	float m_RGuruguruAngle;
	float m_LGuruguruAngle;

	float m_RGuruguruBack[5];
	float m_LGuruguruBack[5];

	float m_RGuruguruPower;
	float m_LGuruguruPower;

	//�ˌ��n
	float m_shootTime;   //���ۂɌv�Z�Ɏg�p������

	float m_shootDelay;  //�ˌ����x��ݒ肷����
	float m_bulletSpeed; //�e�ۂ̑��x
	bool m_isFullAuto;

	//�̗�
	int m_hp;

	//�X�e�[�^�X
	float m_moveSpeed;		//��{�ړ����x
	float m_angleSpeed;		//��]�p���x
	float m_moveForceVel;	//����
	float m_moveForceMax;	//�ő呬�x

	float m_accecelForce; 
};