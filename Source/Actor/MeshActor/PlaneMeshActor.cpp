#include "PlaneMeshActor.h"
#include "PlaneMesh.h"
#include "Game.h"
#include "Renderer.h"

PlaneMeshActor::PlaneMeshActor(class Game* game,
	int shaderClassIndex, const wchar_t* texturePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel,
	float scaleX, float scaleY, float scaleU, float scaleV,
	bool ddsFlag)
	:MeshActor(game, pos, vel, angle, avel)
	, m_scaleX(scaleX)
	, m_scaleY(scaleY)
{
	m_mesh = std::make_unique<PlaneMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, texturePath, scaleX, scaleY, scaleU, scaleV, ddsFlag);
}

PlaneMeshActor::~PlaneActor()
{
}

void PlaneMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void PlaneMeshActor::Draw()
{
	m_mesh->Draw();
}

bool PlaneMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}