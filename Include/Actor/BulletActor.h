#pragma once

#include "SphereActor.h"

class BulletActor : public SphereActor {
public:
	BulletActor(class Game* game, int shaderClassIndex,
		const wchar_t* filePath, float radius, bool isPlayerBullet,int id,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		int effect = 0);

	~BulletActor();

	void UpdateActor(float deltaTime) override;

	void Draw() override;

	int getID() { return m_id; }
	int getEffect() { return m_effect; }
	bool isPlayerBullet() { return m_isPlayerBullet; }
	float getRadius() { return m_radius; }

	Shape::Circle getBoundingCircle() { return Shape::Circle(XMFLOAT2(m_pos.x, m_pos.y), m_radius); }

private:

	float m_radius;
	bool m_isPlayerBullet;
	int m_effect;
	int m_id;


};