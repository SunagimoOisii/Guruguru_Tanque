#include "Actor3D.h"
#include "Game.h"

const float Actor3D::GravitationalAcceleration(9.8f);

Actor3D::Actor3D(class Game* game,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel)
	: Actor(game)
	, m_pos(pos)
	, m_vel(vel)
	, m_angle(angle)
	, m_angularVel(avel)
	, m_pivot(ZeroVec3)
	, m_scale(OneVec3)
	, m_worldTransform(XMMatrixIdentity())
	, m_recalculateWorldTransform(true)
	, m_mass(1.0f)
	, m_enableGravity(true)
	, m_dragCoefficient(0.0f)
	, m_moveState(MoveState::Idle)
	, m_moveForce(100.0f)
	, m_maxSpeed(50.0f)
	, m_jumpSpeed(20.0f)
{
	CalcWorldTransform();
}

Actor3D::~Actor3D()
{
}

//位置や姿勢が変化した際はワールド行列の再計算フラグを立てる

void Actor3D::SetPos(const XMFLOAT3& pos)
{
	m_pos = pos;
	m_recalculateWorldTransform = true;
}

void Actor3D::SetAngle(const XMFLOAT3& angle)
{
	m_angle = angle;
	m_angle.x = (m_angle.x >= 360.0f) ? std::fmod(m_angle.x, 360.0f)
		: ((m_angle.x < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.x), 360.0f) : m_angle.x);
	m_angle.y = (m_angle.y >= 360.0f) ? std::fmod(m_angle.y, 360.0f)
		: ((m_angle.y < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.y), 360.0f) : m_angle.y);
	m_angle.z = (m_angle.z >= 360.0f) ? std::fmod(m_angle.z, 360.0f)
		: ((m_angle.z < 0.0f) ? 360.0f - std::fmod(std::fabs(m_angle.z), 360.0f) : m_angle.z);

	m_recalculateWorldTransform = true;
}

void Actor3D::SetPivot(XMFLOAT3 pivot)
{
	m_pivot = pivot;
	m_recalculateWorldTransform = true;
}

void Actor3D::SetScale(XMFLOAT3 scale)
{
	m_scale = scale;
	m_recalculateWorldTransform = true;
}

XMFLOAT3 Actor3D::GetForward()
{
	CalcWorldTransform();

	float rx = m_worldTransform.r[0].m128_f32[0];
	float ry = m_worldTransform.r[0].m128_f32[1];
	float rz = m_worldTransform.r[0].m128_f32[2];

	return (1.0f / m_scale.x) * XMFLOAT3(rx, ry, rz);
}

XMFLOAT3 Actor3D::GetRight()
{
	CalcWorldTransform();

	float rx = m_worldTransform.r[1].m128_f32[0];
	float ry = m_worldTransform.r[1].m128_f32[1];
	float rz = m_worldTransform.r[1].m128_f32[2];

	return (1.0f / m_scale.y) * XMFLOAT3(rx, ry, rz);
}

XMFLOAT3 Actor3D::GetUp()
{
	CalcWorldTransform();

	float rx = m_worldTransform.r[2].m128_f32[0];
	float ry = m_worldTransform.r[2].m128_f32[1];
	float rz = m_worldTransform.r[2].m128_f32[2];

	return (1.0f / m_scale.z) * XMFLOAT3(rx, ry, rz);
}

void Actor3D::UpdateNode(float deltaTime, const XMMATRIX& parentMat)
{
}

//*************************************************************
//↑public ↓protected
//*************************************************************

void Actor3D::CalcWorldTransform()
{
	if (m_recalculateWorldTransform == false) return;

	//スケール変換
	m_worldTransform = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	//角度の反映
	//平行移動行列でピボットを原点に移動させ、
	//回転変換後に座標を戻すことで任意座標周りでの回転を実現させている
	m_worldTransform *= XMMatrixTranslation(-m_pivot.x, -m_pivot.y, -m_pivot.z);
	m_worldTransform *= XMMatrixRotationX(XMConvertToRadians(m_angle.x));
	m_worldTransform *= XMMatrixRotationY(XMConvertToRadians(m_angle.y));
	m_worldTransform *= XMMatrixRotationZ(XMConvertToRadians(m_angle.z));
	m_worldTransform *= XMMatrixTranslation(m_pos.x + m_pivot.x,
		m_pos.y + m_pivot.y, m_pos.z + m_pivot.z);

	m_recalculateWorldTransform = false;
}

void Actor3D::Translate(float deltaTime)
{
	SetPos(m_pos + m_vel * deltaTime);
}

void Actor3D::Rotate(float deltaTime)
{
	SetAngle(m_angle + m_angularVel * deltaTime);
}

void Actor3D::Simulate(float deltaTime, XMFLOAT3 addForce)
{
	XMFLOAT3 force = addForce;
	if (m_enableGravity)
	{
		force += -GravitationalAcceleration * m_mass * UnitVec3Z;
	}
	force += -m_dragCoefficient * m_vel;

	XMFLOAT3 accel = (1.0f / m_mass) * force;
	m_vel += deltaTime * accel;

	//最大速度チェック
	float speed = Length(m_vel);
	if (speed > m_maxSpeed)
	{
		m_vel = m_maxSpeed * Normalize(m_vel);
	}

	SetPos(m_pos + deltaTime * m_vel);
}