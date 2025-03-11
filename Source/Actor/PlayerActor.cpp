#include "EffectActor.h"
#include "Config.h"
#include "PlayerActor.h"
#include "TerrainActor.h"
#include "Collider.h"
#include "Game.h"
#include "TitleSceneActor.h"

const float PlayerActor::PermissibleHeight(1.0f);

PlayerActor::PlayerActor(Game* game, int shaderClassIndex,
	const wchar_t* filePath, int ID,float scaleLong, float scaleWidth,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel)
	: FileMeshActor(game, shaderClassIndex, filePath, pos, vel, angle, aVel)
	, m_scaleLong(scaleLong)
	, m_scaleWidth(scaleWidth)
	, m_RGuruguruAngle(0.0f)
	, m_RGuruguruPower(0.0f)
	, m_prevFrameRightGuruguruAngle(0.0f)
	, m_LGuruguruAngle(0.0f)
	, m_LGuruguruPower(0.0f)
	, m_prevFrameLeftGuruguruAngle(0.0f)
	, m_ID(ID)
	, m_shootDelay(0.5f)
	, m_shootTime(0.0f)
	, m_bulletSpeed(7.0f)
	, m_isFullAuto(true)
	, m_hp(1)
	, m_moveSpeed(5.0f)			//基本移動速度
	, m_angleSpeed(150.0f)		//回転角速度
	, m_moveForceVel(0.5f)		//加速
	, m_moveForceMax(8.0f)		//最大速度
	, m_accecelForce(1.0f)
{
	m_enableGravity = false;

	//初期座標設定
	SetPos(XMFLOAT3(pos.x, pos.y, 0.0f));
	m_transSpeed   = 50.0f;
	m_angularSpeed = 90.0f;

	//マテリアル設定
	SetOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.02f, 0.02f, 0.02f), 1.0f, XMFLOAT3(0.7f, 0.7f, 0.7f)));

	//カメラ設定
	m_visible = true; //FPVカメラを用いるので非表示に設定

	for (int i = 0; i < 5; i++)
	{
		m_RGuruguruBack[i] = 0.0f;
		m_LGuruguruBack[i] = 0.0f;
	}
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::UpdateActor(float deltaTime)
{
	auto* soundSystem = m_game->GetSoundSystem();
	const Keyboard& keyB   = m_game->GetKeyboard();
	const GamePad& gamePad = m_game->GetGamePad(m_ID);

	//入力による移動値計算
	int vx = 0, vy = 0, vz = 0;
	if (keyB.isDown(VK_UP) || gamePad.isDown(XINPUT_GAMEPAD_DPAD_UP))		vx += 1;
	if (keyB.isDown(VK_DOWN) || gamePad.isDown(XINPUT_GAMEPAD_DPAD_DOWN))   vx -= 1;
	if (keyB.isDown(VK_RIGHT) || gamePad.isDown(XINPUT_GAMEPAD_DPAD_RIGHT)) vy += 1;
	if (keyB.isDown(VK_LEFT) || gamePad.isDown(XINPUT_GAMEPAD_DPAD_LEFT))   vy -= 1;

	//ゲームパッドの角度計算。
	m_prevFrameRightGuruguruAngle = m_RGuruguruAngle;
	m_prevFrameLeftGuruguruAngle = m_LGuruguruAngle;

	XMFLOAT2 Lstick = gamePad.GetLeftStick();
	XMFLOAT2 Rstick = gamePad.GetRightStick();

	float noAngleLong = 0.0;//一定の範囲での回転は無視する。

	float RAngle = 0.0f;
	if (Rstick.x * Rstick.x + Rstick.y * Rstick.y > noAngleLong * noAngleLong)
	{
		RAngle = atan2f(Rstick.y, Rstick.x);
	}
	float LAngle = 0.0f;
	if (Lstick.x * Lstick.x + Lstick.y * Lstick.y > noAngleLong * noAngleLong)
	{
		LAngle = atan2f(Lstick.y, Lstick.x);
	}

	//差の絶対値が3.14以上(3.14と-3.14の間をまたぐ)の場合
	float RAngleDif = 0.0f;
	if ((RAngle - m_RGuruguruAngle > 0 && RAngle - m_RGuruguruAngle > 3.141592) || (RAngle - m_RGuruguruAngle < 0 && RAngle - m_RGuruguruAngle < -3.141592))
	{
		RAngleDif = RAngle + m_RGuruguruAngle;
	}
	else
	{
		RAngleDif = RAngle - m_RGuruguruAngle;
	}
	float LAngleDif = 0.0f;
	if ((LAngle - m_LGuruguruAngle > 0 && LAngle - m_LGuruguruAngle > 3.141592) || (LAngle - m_LGuruguruAngle < 0 && LAngle - m_LGuruguruAngle < -3.141592))
	{
		LAngleDif = LAngle + m_LGuruguruAngle;
	}
	else
	{
		LAngleDif = LAngle - m_LGuruguruAngle;
	}

	m_RGuruguruAngle = RAngle;
	m_LGuruguruAngle = LAngle;

	//過去5フレームの記録
	for (int i = 4; i > 0; --i)
	{
		m_RGuruguruBack[i] = m_RGuruguruBack[i - 1];
		m_LGuruguruBack[i] = m_LGuruguruBack[i - 1];
	}
	m_RGuruguruBack[0] = RAngleDif;
	m_LGuruguruBack[0] = LAngleDif;

	//5フレームの平均を出す
	float rSum =0.0f;
	float lSum =0.0f;
	for (int i = 0; i < 5; i++)
	{
		rSum += m_RGuruguruBack[i];
		lSum += m_LGuruguruBack[i];
	}
	m_RGuruguruPower = rSum / 5.0f;
	m_LGuruguruPower = lSum / 5.0f;
	
	////速度設定など
	float fiexedTurning = 0.2f;	//その場回転のためのしきい値
	//float MoveSpeed = 3.0f;			//基本移動速度
	//float AngleSpeed = 120.0f;		//回転角速度
	//float moveForceVel = 0.5f;		//加速
	//float moveForceMax = 3.0f;		//最大速度

	//速度が一定以下なら進まない。
	float power = 0.0f;
	if ((m_RGuruguruPower - m_LGuruguruPower > 0.0f && m_RGuruguruPower - m_LGuruguruPower > fiexedTurning) || (m_RGuruguruPower - m_LGuruguruPower < 0.0f && m_RGuruguruPower - m_LGuruguruPower < -fiexedTurning))
	{
		power = m_RGuruguruPower - m_LGuruguruPower; //逆方向に回すと大きさを加算した値になる。
		if (m_accecelForce < m_moveForceMax)
		{
			m_accecelForce += m_moveForceVel * deltaTime;
			if (m_accecelForce > m_moveForceMax)
			{
				m_accecelForce = m_moveForceMax;
			}
		}
	}
	else
	{
		m_accecelForce = 1.0f;
	}

	XMFLOAT3 vel = GetForward() * power * m_moveSpeed * m_accecelForce;//aa  m_moveforce
	SetAngle(XMFLOAT3(0.0f, 0.0f, m_angle.z + ((m_RGuruguruPower + m_LGuruguruPower) * m_angleSpeed * deltaTime)));


	//壁との当たり判定。
	bool collideWall = false;
	Circle  boundingCircle = Shape::Circle(XMFLOAT2(m_pos.x + deltaTime * vel.x, m_pos.y + deltaTime * vel.y),getRadius());
	std::vector<WallActor*> walls = m_game->GetWalls();
	for (int i = 0; i < walls.size(); i++)
	{
		if (Collide(walls[i]->GetBoundingRotatedRect(), boundingCircle))
		{
			collideWall = true;
		}
	}

	if (collideWall == false)
	{
		SetPos(m_pos + deltaTime * vel);
		CalcWorldTransform();
	}


	//弾の発射
	if (m_shootTime > 0.0f)
	{
		m_shootTime -= deltaTime;
	}

	if ( ((gamePad.isDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && m_isFullAuto)|| (gamePad.isPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER))) && m_shootTime <= 0.0f)
	{
		m_shootTime = m_shootDelay;

		const wchar_t* texture = nullptr;
		if (m_ID == 0)		texture = RED_TEX;
		else if (m_ID == 1) texture = BLUE_TEX;
		new BulletActor(m_game, Renderer::ShaderClassToon, texture, 0.5f, true, m_ID, m_pos, GetForward() * m_bulletSpeed, 0);

		soundSystem->SetSE(L"sound\\se\\shot.mp3");
	}

	//弾との当たり判定
	RotateRect boundinfRect = GetBoundingRect();
	std::vector<Actor*> actors = m_game->GetActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if (typeid(*actors[i]).name() == typeid(BulletActor).name())
		{
   			BulletActor* bullet = (BulletActor*)actors[i];
			if (bullet->getID() != m_ID)
			{
				if (Collide(boundinfRect, bullet->getBoundingCircle()))
				{
					soundSystem->SetSE(L"sound\\se\\damaged.mp3");
					bullet->SetState(Dead);
					m_hp -= 1;
				}
			}
		}
	}



	FileMeshActor::UpdateActor(deltaTime);
}

void PlayerActor::Draw()
{

	if (!m_visible) return;
	FileMeshActor::Draw();
}


Shape::RotateRect PlayerActor::GetBoundingRect()
{
	return Shape::RotateRect({ m_pos.x, m_pos.y }, m_scaleWidth, m_scaleLong, m_angle.z);
}

void PlayerActor::setMoveStates(float moveSpeed, float angleSpeed, float moveForceVel, float moveForceMax)
{
	m_moveSpeed = moveSpeed;
	m_angleSpeed = angleSpeed;
	m_moveForceVel = moveForceVel;
	m_moveForceMax = moveForceMax;
}