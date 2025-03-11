#include "ContactableActor.h"
#include "Game.h"

ContactableActor::ContactableActor(Game* game, int shaderClassIndex,
	std::vector<const wchar_t*>& texturePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	float scaleX, float scaleY, float scaleZ, bool ddsFlag)
	: BoxActor(game, shaderClassIndex, texturePath, true,
		pos, vel, angle, aVel, scaleX, scaleY, scaleZ, ddsFlag)
{
	m_transSpeed   = 5.0f;
	m_angularSpeed = 90.0f;

	SetOriginalMaterial(MeshMaterial(XMFLOAT3(1.0f, 0.2f, 0.2f),
		XMFLOAT3(0.6f, 0.08f, 0.08f), 8.0f, XMFLOAT3(0.4f, 0.2f, 0.2f)));
}

ContactableActor::~ContactableActor()
{
}

void ContactableActor::UpdateActor(float deltaTime)
{
	Translate(deltaTime);
	Rotate(deltaTime);
	CalcWorldTransform();
}

void ContactableActor::Draw()
{
	BoxActor::Draw();
}