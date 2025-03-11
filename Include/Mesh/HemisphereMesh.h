#pragma once

#include "Mesh.h"

/// <summary>
/// 半球メッシュクラス
/// </summary>
class HemisphereMesh : public Mesh
{
public:
	/// <summary>
	/// テクスチャ : uv座標は0～1を基準とし、ScaleU,Vでテクスチャの繰り返し回数を指定<para></para>
	/// uv座標は魚眼投影によってマッピングする
	/// </summary>
	/// <param name="outer">メッシュの内外どちらから見て表裏とするかの設定</param>
	/// <param name="divisionU,V">球の垂直,水平の分割数</param>
	/// <param name="orthographic">正射影か立体射影か</param>
	HemisphereMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float rad, UINT divisionV, UINT divisionH,
		float circleX, float circleY, float imgRad, float angleScale,
		bool orthographic, bool ddsFlag = false);
	~HemisphereMesh();

	void Draw() override;

private:
	int m_indicesNum;
};