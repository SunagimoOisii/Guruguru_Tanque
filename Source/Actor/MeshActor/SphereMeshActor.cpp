#include "SphereMeshActor.h"
#include "SphereMesh.h"
#include "Game.h"
#include "Renderer.h"

SphereMeshActor::SphereMeshActor(Game* game,
	int shaderClassIndex, const wchar_t* texturePath,
	bool outer, float radius, UINT divisionV, UINT divisionH,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	bool oneSide, float scaleU, float scaleV,
	bool ddsFlag)
	: MeshActor(game, pos, vel, angle, aVel)
	, m_radius(radius)
{
	m_mesh = std::make_unique<SphereMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, texturePath, outer, radius, divisionV, divisionH,
		oneSide, scaleU, scaleV, ddsFlag);
}

SphereMeshActor::~SphereMeshActor()
{
}

void SphereMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void SphereMeshActor::Draw()
{
	m_mesh->Draw();
}

bool SphereMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}