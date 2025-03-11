#include "GridMeshActor.h"
#include "GridMesh.h"
#include "Game.h"

GridMeshActor::GridMeshActor(class Game* game,
	int shaderClassIndex, const wchar_t* texturePath,
	const GridHeightMap& map, float scaleU, float scaleV, 
	bool ddsFlag)
	: MeshActor(game, ZeroVec3, ZeroVec3, ZeroVec3, ZeroVec3)
{
	m_mesh = std::make_unique<GridMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, texturePath, map, scaleU, scaleV, ddsFlag);
	m_map = map;
}

GridMeshActor::~GridMeshActor()
{
}

bool GridMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}

void GridMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void GridMeshActor::Draw()
{
	if (!m_visible) return;

	m_mesh->Draw();
}