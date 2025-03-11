#pragma once

#include "Mesh.h"
#include <vector>

/// <summary>
/// 立方体,直方体メッシュクラス
/// </summary>
class BoxMesh : public Mesh
{
public:
	/// <summary>
	/// スケール : 原点を中心に1m*1m*1m基準で倍率ScaleX,Y,Zを指定<para></para>
	/// テクスチャ : 1or6枚設定可能
	/// </summary>
	/// <param name="outer">メッシュの内外どちらから見て表裏とするかの設定</param>
	BoxMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, std::vector<const wchar_t*>& texturePath,
		bool outer, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		bool ddsFlag = false);
	~BoxMesh();

	void Draw() override;
};