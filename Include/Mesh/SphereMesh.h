#pragma once

#include "Mesh.h"

/// <summary>
/// 球メッシュクラス
/// </summary>
class SphereMesh : public Mesh
{
public:
	/// <summary>
	/// テクスチャ : uv座標は0～1を基準とし、ScaleU,Vでテクスチャの繰り返し回数を指定<para></para>
	/// uv座標は球座標投影によってマッピングする
	/// </summary>
	/// <param name="outer">メッシュの内外どちらから見て表裏とするかの設定</param>
	/// <param name="divisionU,V">球の垂直,水平の分割数</param>
	SphereMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float rad, UINT divisionV, UINT divisionH,
		bool oneSide = false, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~SphereMesh();

	void Draw() override;

private:
	int m_indicesNum;
};