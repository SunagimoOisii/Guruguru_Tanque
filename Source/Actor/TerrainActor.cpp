#include "TerrainActor.h"
#include "Game.h"

TerrainActor::TerrainActor(Game* game,
	int shaderClassIndex, const wchar_t* texturePath,
	const GridHeightMap& map, float scaleU, float scaleV, bool ddsFlag)
	: GridMeshActor(game, shaderClassIndex, texturePath,
		map, scaleU, scaleV, ddsFlag)
{
}

TerrainActor::~TerrainActor()
{
}

void TerrainActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void TerrainActor::Draw()
{
	GridMeshActor::Draw();
}