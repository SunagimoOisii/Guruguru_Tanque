#pragma once

#include "Actor.h"

class Actor3D : public Actor
{
public:
	Actor3D(class Game* game,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3 & angle = ZeroVec3, const XMFLOAT3 & avel = ZeroVec3);
	virtual ~Actor3D();

/**********************************
ゲッターセッター
**********************************/

//位置角度スケール

	void SetPos(const XMFLOAT3& pos);
	XMFLOAT3 GetPos() const { return m_pos; }
	void SetVel(const XMFLOAT3& vel) { m_vel = vel; }
	XMFLOAT3 GetVel() const { return m_vel; }
	void SetAngle(const XMFLOAT3& angle);
	XMFLOAT3 GetAngle() const { return m_angle; }
	void SetAngularVel(const XMFLOAT3& avel) { m_angularVel = avel; }
	XMFLOAT3 GetAngularVel() const { return m_angularVel; }
	void SetPivot(XMFLOAT3 pivot);
	XMFLOAT3 GetPivot() const { return m_pivot; }
	void SetScale(XMFLOAT3 scale);
	XMFLOAT3 GetScale() const { return m_scale; }

//ローカル軸方向ベクトル

	XMFLOAT3 GetForward();
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();

	XMMATRIX GetWorldMatrix()const { return m_worldTransform; }

//物理演算

	void SetMass(float mass) { m_mass = mass; }
	float GetMass() const { return m_mass; }
	void SetEnableGravity(bool flag) { m_enableGravity = flag; }
	bool IsEnableGravity() const { return m_enableGravity; }
	void SetDragCoefficient(float coef) { m_dragCoefficient = coef; }
	float GetDragCoefficient() const { return m_dragCoefficient; }

//動作処理

	enum class MoveState
	{
		Idle,
		Walk,
		Jump
	};
	void SetMoveState(MoveState state) { m_moveState = state; }
	MoveState GetMoveState() const { return m_moveState; }
	void SetMoveForce(float force) { m_moveForce = force; }
	float GetMoveForce() const { return m_moveForce; }
	void SetMaxSpeed(float speed) { m_maxSpeed = speed; }
	float GetMaxSpeed() const { return m_maxSpeed; }
	void SetJumpSpeed(float speed) { m_jumpSpeed = speed; }
	float SetJumpSpeed() const { return m_jumpSpeed; }

/**********************************
関数
**********************************/
	
	/// <summary>
	/// 階層構造の更新用関数
	/// </summary>
	/// <param name="parentMat">親ノードの変換行列</param>
	virtual void UpdateNode(float deltaTime, const XMMATRIX& parentMat);

	static const float GravitationalAcceleration;

protected:

	void CalcWorldTransform();

	void Translate(float deltaTime);
	void Rotate(float deltaTime);

	void Simulate(float deltaTime, XMFLOAT3 addForce = ZeroVec3);

/**********************************
↑関数　↓変数　(protected)
**********************************/

//位置角度スケール

	XMFLOAT3 m_pos;
	XMFLOAT3 m_vel;
	XMFLOAT3 m_angle;
	XMFLOAT3 m_angularVel;
	XMFLOAT3 m_pivot;
	XMFLOAT3 m_scale;
	XMMATRIX m_worldTransform;
	bool	 m_recalculateWorldTransform;

//物理演算

	float m_mass;
	bool  m_enableGravity;
	float m_dragCoefficient;

//動作処理

	MoveState m_moveState;
	float	  m_moveForce;
	float	  m_maxSpeed;
	float	  m_jumpSpeed;
};