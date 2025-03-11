#include "BulletActor.h"
#include "WallActor.h"
#include "Game.h"

BulletActor::BulletActor(Game* game, int shaderClassIndex,
	const wchar_t* filePath, float radius, bool isPlayerBullet, int id,
	const XMFLOAT3& pos, const XMFLOAT3& vel, int effect)
	: SphereActor(game, shaderClassIndex,filePath,true,radius,10,10,pos,vel),
	m_isPlayerBullet(isPlayerBullet),
	m_effect(effect),
	m_radius(radius),
	m_id(id)
{

}

BulletActor::~BulletActor() {

}

void BulletActor::UpdateActor(float deltaTime) {

	//à⁄ìÆ
	SetPos(m_pos + deltaTime * m_vel);
	CalcWorldTransform();

	//ï«Ç…ìñÇΩÇ¡ÇΩÇÁè¡Ç∑
	Shape::Circle boundingCircle = getBoundingCircle();
	std::vector<WallActor*> walls = m_game->GetWalls();
	for (int i = 0; i < walls.size(); i++)
	{
		if (Collide(walls[i]->GetBoundingRotatedRect(),boundingCircle))
		{
			m_state = State::Dead;
		}
	}
}

void BulletActor::Draw() {
	SphereActor::Draw();
}