#include "HemisphereMeshActor.h"
#include "HemisphereMesh.h"
#include "Game.h"
#include "Renderer.h"

HemisphereMeshActor::HemisphereMeshActor(Game* game,
	int shaderClassIndex, const wchar_t* texturePath,
	bool outer, float rad, UINT divisionV, UINT divisionH,
	float circleX, float circleY, float imgRad,
	float angleScale, bool orthographic,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	bool ddsFlag)
	:MeshActor(game, pos, vel, angle, aVel)
{
	m_mesh = std::make_unique<HemisphereMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, texturePath, outer, rad, divisionV, divisionH,
		circleX, circleY, imgRad, angleScale, orthographic, ddsFlag);
}

HemisphereMeshActor::~HemisphereActor()
{

}

void HemisphereMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void HemisphereMeshActor::Draw()
{
	m_mesh->Draw();
}

bool HemisphereMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}