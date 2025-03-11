#pragma once

#include "MeshActor.h"

class GridMeshActor : public MeshActor
{
public:
	GridMeshActor(class Game* game,
		int shaderClassIndex, const wchar_t* texturePath,
		const GridHeightMap& map, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~GridMeshActor();

	bool IsEnabled() const;
	const GridHeightMap& GetBoundingGridHeightMap() const { return m_map; }

	void UpdateActor(float deltaTime) override;

	void Draw() override;

protected:
	GridHeightMap m_map;
};