#pragma once

#include "Math.h"
#include "GameUtil.h"
using namespace Shape;

/**********************************
円との衝突判定
**********************************/

bool Collide(const Circle& c, const XMFLOAT2& p);

bool Collide(const Circle& c1, const Circle& c2);

bool Collide(const Circle& c, const Segment& s);

/**********************************
矩形との衝突判定
**********************************/

bool Collide(const Shape::Rect& rect, const XMFLOAT2& p);

bool Collide(const Shape::Rect& rect1, const Shape::Rect& rect2);

bool Collide(const Shape::Rect& rect, const Circle& c);

bool Collide(const Shape::RotateRect& rect, const Circle& c);

/**********************************
球との衝突判定
**********************************/

bool Collide(const Sphere& s, const XMFLOAT3& p);

bool Collide(const Sphere& s1, const Sphere& s2);

/// <summary>
/// s1を固定と考え、めり込み補正を行う
/// </summary>
/// <param name="corrDir">s1をめり込み補正する方向ベクトル</param>
/// <param name="len">移動量</param>
bool Collide(const Sphere& s1, const Sphere& s2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

bool Collide(const Sphere& sphere, const OBB& obb,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
平面との衝突判定
**********************************/

/// <summary>
/// planeを固定と考え、sphereにめり込み補正を行う
/// </summary>
/// <param name="corrDir">s1をめり込み補正する方向ベクトル</param>
/// <param name="len">移動量</param>
bool Collide(const Sphere& sphere, const InfinitePlane& plane,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
AABBとの衝突判定
**********************************/

/// <summary>
/// b2を固定と考え、めり込み補正を行う<para></para>
/// めり込み補正量の計算が不要な場合はcorrDir,lenはnullptrを設定
/// </summary>
/// <param name="corrDir">b2をめり込み補正する方向ベクトル</param>
/// <param name="len">移動量</param>
bool Collide(const AABB& b1, const AABB& b2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
OBBとの衝突判定
**********************************/

/// <summary>
/// めり込み補正量の計算が不要な場合はcorrDir,lenはnullptrを設定
/// </summary>
/// <param name="corrDir">b2をめり込み補正する方向ベクトル</param>
/// <param name="len">移動量</param>
bool Collide(const OBB& b1, const OBB& b2,
	XMFLOAT3* corrDir = nullptr, float* length = nullptr);

/**********************************
Rayとの衝突判定
**********************************/

/// <param name="vert0,1,2">三角形の頂点</param>
/// <param name="rho">衝突時の媒介変数を保存する変数のポインタ</param>
/// <param name="cull">カリング有効フラグ 有効時、三角形裏面へのレイを計算しない</param>
/// <returns></returns>
bool Collide(const Ray& ray, const XMFLOAT3& vert0,
	const XMFLOAT3& vert1, const XMFLOAT3& vert2, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const OBB& obb, float* rho = nullptr);

bool Collide(const Ray& ray, const InfinitePlane& plane, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const FinitePlane& plane, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const Sphere& sphere, float* rho = nullptr, bool cull = true);

bool Collide(const Ray& ray, const GridHeightMap& map, float* rho = nullptr);

