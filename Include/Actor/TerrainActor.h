#pragma once

#include "GridMeshActor.h"

/// <summary>
/// GridMeshActorを基底クラスとして、地形を表現するActor
/// </summary>
class TerrainActor : public GridMeshActor
{
public:
	TerrainActor(class Game* game,
		int shaderClassIndex, const wchar_t* texturePath,
		const GridHeightMap& map, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~TerrainActor();

	virtual void UpdateActor(float deltaTime) override;

	virtual void Draw() override;
};