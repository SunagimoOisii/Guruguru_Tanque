#define NOMINMAX
#include "Collider.h"
#include <cmath>
#include <algorithm>

/**********************************
円との衝突判定
**********************************/

bool Collide(const Circle& c, const XMFLOAT2& p)
{
	XMFLOAT2 d = p - c.pos;
	return LengthSq(d) <= c.r * c.r;
}

bool Collide(const Circle& c1, const Circle& c2)
{
	XMFLOAT2 d = c2.pos - c1.pos;
	float csq = (c1.r + c2.r) * (c1.r + c2.r);
	return LengthSq(d) <= csq;
}

bool Collide(const Circle& c, const Segment& s)
{
	XMFLOAT2 p = s.end - s.start;
	XMFLOAT2 q = c.pos - s.start;
	float d = std::fabs(Cross(p, q)) / Length(p);
	if (d > c.r) return false;

	XMFLOAT2 w = c.pos - s.end;
	float t = Dot(p, q) * Dot(-1.0f * p, w);
	if (t >= 0.0f) return true;

	float csq = c.r * c.r;
	if (LengthSq(q) <= csq) return true;
	if (LengthSq(w) <= csq) return true;
	return false;
}
/**********************************
矩形との衝突判定
**********************************/

bool Collide(const Rect& rect, const XMFLOAT2& p)
{
	if (p.x < rect.left)   return false;
	if (rect.right < p.x)  return false;
	if (p.y < rect.left )  return false;
	if (rect.bottom < p.y) return false;
	return true;
}

bool Collide(const Rect& rect1, const Rect& rect2)
{
	if (rect1.right < rect2.left) return false;
	if (rect2.left < rect1.right) return false;
	if (rect1.bottom < rect2.top) return false;
	if (rect2.top < rect1.bottom) return false;
	return true;
}

bool Collide(const Rect& rect, const Circle& c)
{
	XMFLOAT2 p = c.pos;
	p.x = (rect.right < p.x) ? rect.right : p.x;
	p.x = (rect.left > p.x) ? rect.left : p.x;
	p.y = (rect.bottom < p.y) ? rect.bottom : p.y;
	p.y = (rect.top > p.y) ? rect.top : p.y;

	float csq = c.r * c.r;
	return LengthSq(p - c.pos) <= csq;
}

XMFLOAT2 RotatePoint(const XMFLOAT2& point, float angle) {
	float rad = angle * XM_PI / 180.0f;
	float cosA = cos(rad);
	float sinA = sin(rad);
	return { point.x * cosA - point.y * sinA, point.x * sinA + point.y * cosA };
}

// 回転矩形と円の衝突判定
bool Collide(const RotateRect& rect, const Circle& c) {
	// 円の座標を変換
	XMFLOAT2 localCirclePos = { c.pos.x - rect.pos.x, c.pos.y - rect.pos.y };
	localCirclePos = RotatePoint(localCirclePos, -rect.angle);
	
	float closestX = std::max(-rect.width / 2.0f, std::min(localCirclePos.x, rect.width / 2.0f));
	float closestY = std::max(-rect.height / 2.0f, std::min(localCirclePos.y, rect.height / 2.0f));
	float dx = localCirclePos.x - closestX;
	float dy = localCirclePos.y - closestY;

	return (dx * dx + dy * dy) <= (c.r * c.r);
}


/**********************************
球との衝突判定(内外判定)
**********************************/

bool Collide(const Sphere& s, const XMFLOAT3& p)
{
	XMFLOAT3 distance = p - s.pos;
	
	return LengthSq(distance) <= s.r * s.r;
}

bool Collide(const Sphere& s1, const Sphere& s2)
{
	XMFLOAT3 distance = s1.pos - s2.pos;
	float sq = (s1.r + s2.r) * (s1.r + s2.r);

	return LengthSq(distance) <= sq;
}

bool Collide(const Sphere& s1, const Sphere& s2,
	XMFLOAT3* corrDir, float* length)
{
	//球同士の距離,２球の半径の二乗を計算
	XMFLOAT3 distance = s1.pos - s2.pos;
	float sq = (s1.r + s2.r) * (s1.r + s2.r);
	float distanceLen = LengthSq(distance);

	//球同士の距離の方が長いか否かで当たり判定を下す
	if (LengthSq(distance) > sq)
	{
		return false;
	}
	else
	{
		if (corrDir != nullptr) *corrDir = Normalize(distance);
		if (length != nullptr)	*length	 = s1.r + s2.r - std::sqrt(distanceLen);
	}

	return distanceLen <= sq;
}

bool Collide(const Sphere& sphere, const OBB& obb,
	XMFLOAT3* corrDir, float* length)
{
	//OBBの球に対する境界上の最近傍点と球の距離と
	//球の半径を比較することで衝突判定を行う
	XMFLOAT3 direction[3] = { obb.dirX,obb.dirY,obb.dirZ };
	float	 scale[3]	  = { obb.scale.x,obb.scale.y,obb.scale.z };
	XMFLOAT3 neighborP	  = obb.pos; //近傍点
	XMFLOAT3 distance	  = sphere.pos - obb.pos;
	bool	 inside		  = true;  //球の中心がOBB内部にあるか否か
	float	 width[3];

	//内積で各軸方向に投影しOBBの幅を超えた場合は調整
	for (int i = 0;i < 3;i++)
	{
		width[i] = Dot(distance, direction[i]);
		if (width[i] > scale[i])
		{
			width[i] = scale[i];
			inside	 = false;
		}
		if (width[i] < -scale[i])
		{
			width[i] = -scale[i];
			inside	 = false;
		}
		neighborP += width[i] * direction[i];
	}

	XMFLOAT3 c = ZeroVec3;
	float v = 0.0f;
	if (inside)
	{
		v = FLT_MAX;
		for (int i = 0;i < 3;i++)
		{
			float p = scale[i] - std::abs(width[i]);
			if (v >= p)
			{
				v = p + sphere.r;
				if (width[i] >= 0.0f)
				{
					c = direction[i];
				}
				else
				{
					c = -1.0f * direction[i];
				}
			}
		}
	}
	else
	{
		XMFLOAT3 diff = sphere.pos - neighborP;
		float diffLen = Length(diff);
		if (diffLen > sphere.r) return false;
		c = Normalize(diff);
		v = sphere.r - diffLen;
	}

	if (corrDir != nullptr)
	{
		*corrDir = c;
	}
	if (length != nullptr)
	{
		*length = v;
	}

	return true;
}

/**********************************
平面との衝突判定
**********************************/

bool Collide(const Sphere& sphere, const InfinitePlane& plane,
	XMFLOAT3* corrDir, float* length)
{
	float distance = Dot(plane.normal, sphere.pos) - plane.distance;
	if (std::abs(distance) > sphere.r) return false;

	if (corrDir != nullptr)
	{
		*corrDir = plane.normal;
	}
	if (length != nullptr)
	{
		*length = sphere.r - distance;
	}

	return true;
}

/**********************************
AABBとの衝突判定
**********************************/

bool Collide(const AABB& b1, const AABB& b2,
	XMFLOAT3* corrDir, float* length)
{
	//分離軸定理に基づいて衝突判定を行う
	if (b1.curMax.x < b2.curMin.x) return false;
	if (b2.curMax.x < b1.curMin.x) return false;
	if (b1.curMax.y < b2.curMin.y) return false;
	if (b2.curMax.y < b1.curMin.y) return false;
	if (b1.curMax.z < b2.curMin.z) return false;
	if (b2.curMax.z < b1.curMin.z) return false;

	//衝突判定のみ行い、めり込み補正の計算が不要な場合
	if (corrDir == nullptr && length == nullptr) return true;

	//以降めり込み補正計算を行う
	//2つのバウンディングボックスの距離を２通りで計算
	XMFLOAT3 diff1 = b2.curMax - b1.curMin;
	XMFLOAT3 diff2 = b2.curMin - b1.curMax;
	XMFLOAT3 d{};
	//２通りのうち距離が短い(偽陽性が低い)方を位置差分として採用
	d.x = (std::fabs(diff1.x) < std::fabs(diff2.x)) ? diff1.x : diff2.x;
	d.y = (std::fabs(diff1.y) < std::fabs(diff2.y)) ? diff1.y : diff2.y;
	d.z = (std::fabs(diff1.z) < std::fabs(diff2.z)) ? diff1.z : diff2.z;

	XMFLOAT3 corr;
	float len;
	XMFLOAT3 absD(std::abs(d.x), std::abs(d.y), std::abs(d.z));
	if (absD.x <= absD.y)
	{
		if (absD.x <= absD.z)
		{
			corr = UnitVec3X;
			len  = d.x;
		}
		else
		{
			corr = UnitVec3Z;
			len  = d.z;
		}
	}
	else if (absD.y <= absD.z)
	{
		corr = UnitVec3Y;
		len  = d.y;
	}
	else
	{
		corr = UnitVec3Z;
		len  = d.z;
	}

	if (corrDir != nullptr)
	{
		*corrDir = corr;
	}
	if (length != nullptr)
	{
		*length = len;
	}

	return true;
}

/**********************************
OBBとの衝突判定
**********************************/

bool Collide(const OBB& b1, const OBB& b2,
	XMFLOAT3* corrDir, float* length)
{
	XMFLOAT3 bx1 = b1.dirX * b1.scale.x;
	XMFLOAT3 by1 = b1.dirY * b1.scale.y;
	XMFLOAT3 bz1 = b1.dirZ * b1.scale.z;
	XMFLOAT3 bx2 = b2.dirX * b1.scale.x;
	XMFLOAT3 by2 = b2.dirY * b1.scale.y;
	XMFLOAT3 bz2 = b2.dirZ * b1.scale.z;
	XMFLOAT3 dir = b1.pos - b2.pos;

	//以降で分離軸判定を行う
	//軸:b1x
	XMFLOAT3 sep = b1.dirX;
	float d1 = b1.scale.x;
	float d2 = CalcLengthProjAxis(sep, bx2, by2, bz2);
	float w  = Dot(dir, sep);
	float wa = std::abs(w);
	if (wa > d1 + d2) return false;
	XMFLOAT3 corr = sep;
	float v, vMin;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	vMin = v;

	//軸:b1y
	sep = b1.dirY;
	d1  = b1.scale.y;
	d2  = CalcLengthProjAxis(sep, bx2, by2, bz2);
	w   = Dot(dir, sep);
	wa  = std::abs(w);
	if (wa > d1 + d2) return false;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	if (std::abs(vMin) > std::abs(v))
	{
		corr = sep;
		vMin = v;
	}

	//軸:b1z
	sep = b1.dirZ;
	d1  = b1.scale.z;
	d2  = CalcLengthProjAxis(sep, bx2, by2, bz2);
	w   = Dot(dir, sep);
	wa  = std::abs(w);
	if (wa > d1 + d2) return false;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	if (std::abs(vMin) > std::abs(v))
	{
		corr = sep;
		vMin = v;
	}

	//軸:b2x
	sep = b2.dirX;
	d1  = CalcLengthProjAxis(sep, bx1, by1, bz1);
	d2  = b2.scale.x;
	w   = Dot(dir, sep);
	wa  = std::abs(w);
	if (wa > d1 + d2) return false;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	if (std::abs(vMin) > std::abs(v))
	{
		corr = sep;
		vMin = v;
	}

	//軸:b2y
	sep = b2.dirY;
	d1  = CalcLengthProjAxis(sep, bx1, by1, bz1);
	d2  = b2.scale.y;
	w   = Dot(dir, sep);
	wa  = std::abs(w);
	if (wa > d1 + d2) return false;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	if (std::abs(vMin) > std::abs(v))
	{
		corr = sep;
		vMin = v;
	}

	//軸:b2z
	sep = b2.dirZ;
	d1  = CalcLengthProjAxis(sep, bx1, by1, bz1);
	d2  = b2.scale.z;
	w   = Dot(dir, sep);
	wa  = std::abs(w);
	if (wa > d1 + d2) return false;
	if (w < 0.0f) v = wa - (d1 + d2);
	else		  v = (d1 + d2) - wa;
	if (std::abs(vMin) > std::abs(v))
	{
		corr = sep;
		vMin = v;
	}

	//軸:外積軸
	XMFLOAT3 s1[3] = { b1.dirX,b1.dirY,b1.dirZ };
	XMFLOAT3 s2[3] = { b2.dirX,b2.dirY,b2.dirZ };
	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			XMFLOAT3 s = Cross(s1[i], s2[j]);
			if (Length(s) <= FLT_EPSILON) continue;
			sep = Normalize(s);
			d1 = CalcLengthProjAxis(sep, bx1, by1, bz1);
			d2 = CalcLengthProjAxis(sep, bx2, by2, bz2);
			w  = Dot(dir, sep);
			wa = std::abs(w);
			if (wa > d1 + d2) return false;
			if (w < 0.0f) v = wa - (d1 + d2);
			else		  v = (d1 + d2) - wa;
			if (std::abs(vMin) > std::abs(v))
			{
				corr = sep;
				vMin = v;
			}
		}
	}

	if (corrDir != nullptr)
	{
		*corrDir = corr;
	}
	if (length != nullptr)
	{
		*length = vMin;
	}

	return true;
}

/**********************************
Rayとの衝突判定
**********************************/

bool Collide(const Ray& ray, const XMFLOAT3& vert0,
	const XMFLOAT3& vert1, const XMFLOAT3& vert2, float* rho, bool cull)
{
	XMFLOAT3 triVert1 = vert1 - vert0;
	XMFLOAT3 triVert2 = vert2 - vert0;
	XMFLOAT3 e   = Cross(ray.dir, triVert2);
	float	 det = Dot(triVert1, e);

	//Rayが面の真横や裏面を向いている場合、判定終了
	if (std::abs(det) < FLT_EPSILON ||
		(cull && det <= 0.0f)) return false;
	
	XMFLOAT3 d = ray.pos - vert0;
	float	 s = Dot(d, e) / det;
	if (s < 0.0f || s > 1.0f) return false;

	XMFLOAT3 f = Cross(d, triVert1);
	float t = Dot(ray.dir, f) / det;
	if (t < 0.0f || t > 1.0f ||
		s + t > 1.0f) return false;

	if (rho != nullptr)
	{
		*rho = Dot(triVert2, f) / det;
	}

	return true;
}

bool Collide(const Ray& ray, const OBB& obb, float* rho)
{
	//OBBの各面を構成する三角形ポリゴンの計算
	XMFLOAT3 p[18] =
	{
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x - obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x - obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x - obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x - obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x + obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x + obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x - obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x - obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x + obb.dirY * obb.scale.y + obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x - obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x - obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos - obb.dirX * obb.scale.x + obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
		obb.pos + obb.dirX * obb.scale.x + obb.dirY * obb.scale.y - obb.dirZ * obb.scale.z,
	};

	float r;
	for (int i = 0;i < 8;i++)
	{
		if (i == 4 || i == 6) continue;

		int j = i * 2;
		if (Collide(ray, p[j], p[j + 2], p[j + 3], &r, true))
		{
			if (rho != nullptr) *rho = r;
			return true;
		}
		if (Collide(ray, p[j], p[j + 3], p[j + 2], &r, true))
		{
			if (rho != nullptr) *rho = r;
			return true;
		}
	}

	return false;
}

bool Collide(const Ray& ray, const InfinitePlane& plane, float* rho, bool cull)
{
	float a = Dot(ray.dir, plane.normal);
	if (cull && a > 0.0f) return false;
	if (std::abs(a) < FLT_EPSILON) return false;

	float r = -(Dot(ray.pos, plane.normal) + plane.distance) / a;
	if (rho != nullptr) *rho = r;

	return true;
}

bool Collide(const Ray& ray, const FinitePlane& plane, float* rho, bool cull)
{
	if (Collide(ray, plane.pos[0], plane.pos[1], plane.pos[3], rho, cull)) return true;
	if (Collide(ray, plane.pos[1], plane.pos[2], plane.pos[3], rho, cull)) return true;

	return false;
}

bool Collide(const Ray& ray, const Sphere& sphere, float* rho, bool cull)
{
	XMFLOAT3 distance = ray.pos - sphere.pos;
	float rp = Dot(ray.dir, distance);
	float pp = Dot(distance, distance);

	if (cull && pp <= sphere.r * sphere.r) return false;

	float b	  = 2.0f * rp;
	float dis = b * b - 4.0f * (pp - sphere.r * sphere.r);
	if (dis < FLT_EPSILON) return false;

	float rhoMin = (-b - std::sqrt(dis)) * 0.5f;
	if (rhoMin > 0.0f)
	{
		if (rho != nullptr) *rho = rhoMin;
		return true;
	}
	if (cull) return false;

	float rhoMax = (-b + std::sqrt(dis)) * 0.5f;
	if (rhoMax < 0.0f)  return false;
	if (rho != nullptr) *rho = rhoMax;

	return true;
}

bool Collide(const Ray& ray, const GridHeightMap& map, float* rho)
{
	if (ray.pos.x < map.minPos.x || ray.pos.x > map.maxPos.x) return false;
	if (ray.pos.y < map.minPos.y || ray.pos.y > map.maxPos.y) return false;

	int x = (int)((ray.pos.x - map.minPos.x) / map.gridWidth.x);
	int y = (int)((ray.pos.y - map.minPos.y) / map.gridWidth.y);

	int index = x + y * map.xVertNum;
	if (Collide(ray, map.heightMap[index], map.heightMap[index + 1],
		map.heightMap[index + 1 + map.xVertNum], rho)) return true;
	if (Collide(ray, map.heightMap[index], map.heightMap[index + 1 + map.xVertNum],
		map.heightMap[index + map.xVertNum], rho)) return true;

	return false;
}