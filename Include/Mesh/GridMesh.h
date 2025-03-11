#pragma once

#include "Mesh.h"

/// <summary>
/// GridHeightMap構造体から格子状のメッシュを生成
/// </summary>
class GridMesh : public Mesh
{
public:
	GridMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		const GridHeightMap& map, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~GridMesh();

	void Draw() override;

private:
	int m_indicesNum;
};