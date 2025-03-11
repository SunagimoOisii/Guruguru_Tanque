#include "BoxMeshActor.h"
#include "BoxMesh.h"
#include "Game.h"
#include "Renderer.h"

BoxMeshActor::BoxMeshActor(class Game* game, int shaderClassIndex,
	std::vector<const wchar_t*>& texturePath, bool outer,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	float scaleX, float scaleY, float scaleZ,
	bool ddsFlag)
	: MeshActor(game, pos, vel, angle, aVel)
	, m_scale(0.5f * scaleX, 0.5f * scaleY, 0.5f * scaleZ)
{
	m_mesh = std::make_unique<BoxMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, texturePath, outer, scaleX, scaleY, scaleZ, ddsFlag);
}

BoxMeshActor::~BoxMeshActor()
{
}

void BoxMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void BoxMeshActor::Draw()
{
	m_mesh->Draw();
}

bool BoxMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}