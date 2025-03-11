#pragma once

#include "Mesh.h"

/// <summary>
/// 平面の矩形メッシュクラス
/// </summary>
class PlaneMesh : public Mesh
{
public:
	/// <summary>
	/// スケール : XY平面上に配置しサイズは1m*1m基準で倍率ScaleX,Y,Zを指定<para></para>
	/// テクスチャ : uv座標は0～1を基準とし、ScaleU,Vでテクスチャの繰り返し回数を指定
	/// </summary>
	PlaneMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~PlaneMesh();

	void Draw() override;
};