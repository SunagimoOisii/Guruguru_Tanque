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

	//射撃系
	float m_shootTime;   //実際に計算に使用するやつ

	float m_shootDelay;  //射撃速度を設定するやつ
	float m_bulletSpeed; //弾丸の速度
	bool m_isFullAuto;

	//体力
	int m_hp;

	//ステータス
	float m_moveSpeed;		//基本移動速度
	float m_angleSpeed;		//回転角速度
	float m_moveForceVel;	//加速
	float m_moveForceMax;	//最大速度

	float m_accecelForce; 
};