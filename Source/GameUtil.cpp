#include "GameUtil.h"
#include "Actor3D.h"

using namespace Light;
using namespace Shape;

//単色テクスチャの名前定義

extern const wchar_t* WHITE_TEX   = L"White";
extern const wchar_t* GRAY_TEX    = L"GRAY";
extern const wchar_t* BLACK_TEX	  = L"Black";
extern const wchar_t* RED_TEX	  = L"Red";
extern const wchar_t* GREEN_TEX	  = L"Green";
extern const wchar_t* BLUE_TEX	  = L"Blue";
extern const wchar_t* YELLOW_TEX  = L"Yellow";
extern const wchar_t* CYAN_TEX	  = L"Cyan";
extern const wchar_t* MAGENTA_TEX = L"Magenta";

/**********************************
ライト構造体
**********************************/

Parallel::Parallel()
	:direction(-1.0f * UnitVec3Z)
	, color(OneVec3)
	, padA(0.0f)
	, padB(0.0f)
{
}

Parallel::Parallel(XMFLOAT3 dir, XMFLOAT3 col)
	:direction(dir)
	, color(col)
	, padA(0.0f)
	, padB(0.0f)
{
}

Point::Point()
	: pos(ZeroVec3)
	, pad(0.0f)
	, color(OneVec3)
	, radius(1.0f)
{
}

Point::Point(XMFLOAT3 p, XMFLOAT3 col, float r)
	: pos(p)
	, pad(0.0f)
	, color(col)
	, radius(r)
{
}

Spot::Spot()
	: pos(ZeroVec3)
	, radius(1.0f)
	, direction(-1.0f * UnitVec3Z)
	, radiationAngle(30.0f)
	, color(OneVec3)
	, pad(0.0f)
{
}

Spot::Spot(XMFLOAT3 p, float r, XMFLOAT3 dir, float angle, XMFLOAT3 col)
	: pos(p)
	, radius(r)
	, direction(dir)
	, radiationAngle(angle)
	, color(col)
	, pad(0.0f)
{
}

Ambient::Ambient()
	: color(OneVec3)
	, pad(0.0f)
{
}

Ambient::Ambient(XMFLOAT3 col)
	: color(col)
	, pad(0.0f)
{
}

/**********************************
図形構造体
**********************************/

Segment::Segment()
	: start(ZeroVec2)
	, end(UnitVec2X)
{
}

Segment::Segment(const XMFLOAT2& a, const XMFLOAT2& b)
	: start(a)
	, end(b)
{
}

Circle::Circle()
	: pos(ZeroVec2)
	, r(1.0f)
{
}

Circle::Circle(const XMFLOAT2& p, float rad)
	: pos(p)
	, r(rad)
{
}

Rect::Rect() 
	: left(0.0f)
	, top(0.0f)
	, right(0.0f)
	, bottom(0.0f) 
{
}

Rect::Rect(float lt, float tp, float rt, float bm)
	: left(lt)
	, top(tp)
	, right(rt)
	, bottom(bm)
{
}

RotateRect::RotateRect()
	: pos(ZeroVec2)
	, width(0)
	, height(0)
	, angle(0)
{}

RotateRect::RotateRect(XMFLOAT2 pos, float width, float height, float angle)
	: pos(pos)
	, width(width)
	, height(height)
	, angle(angle)
{}

Sphere::Sphere() 
	: pos(ZeroVec3)
	, r(1.0f) 
{
}

Sphere::Sphere(const XMFLOAT3& p, float radius)
	: pos(p)
	, r(radius) 
{
}

InfinitePlane::InfinitePlane()
	: normal(UnitVec3Z)
	, distance(0.0f)
{
}

InfinitePlane::InfinitePlane(XMFLOAT3 nor, float dist)
	: normal(nor)
	, distance(dist)
{
}

InfinitePlane::InfinitePlane(XMFLOAT3 nor, XMFLOAT3 pos)
	: normal(nor)
	, distance(-Dot(nor, pos))
{
}

FinitePlane::FinitePlane()
	: pos{ XMFLOAT3(0.5f,0.5f,0.0f),XMFLOAT3(-0.5f,0.5f,0.0f),
	XMFLOAT3(-0.5f,-0.5f,0.0f),XMFLOAT3(0.5f,-0.5f,0.0f) }
{
}

FinitePlane::FinitePlane(XMFLOAT3* p)
	: pos{ p[0],p[1],p[2],p[3] }
{
}

FinitePlane::FinitePlane(const XMFLOAT3& cPos,
	const XMFLOAT3& xVec, const XMFLOAT3& yVec, float scaleX, float scaleY)
	: pos{ cPos + 0.5f * xVec + 0.5f * yVec,cPos - 0.5f * xVec + 0.5f * yVec,
	cPos - 0.5f * xVec - 0.5f * yVec,cPos + 0.5f * xVec - 0.5f * yVec }
{
}

ActorNode::ActorNode()
	: parent(nullptr)
	, actor(nullptr)
{
}

ActorNode::ActorNode(Actor3D* actor)
	: parent(nullptr)
	, actor(actor)
{
}

void ActorNode::AddNode(ActorNode* node)
{
	node->parent = this;
	children.emplace_back(node);
}


void UpdateActorNode(ActorNode* node, float deltaTime)
{
	//末端ノードの場合は終了
	if (node == nullptr) return;

	//自身がルートノードなら自身をUpdateで処理
	//(でないならUpdateActorNodeで処理)
	if (node->parent != nullptr)
	{
		XMMATRIX parentMat = node->parent->actor->GetWorldMatrix();
		node->actor->UpdateNode(deltaTime, parentMat);
	}
	//行きがけ順の再帰処理で子ノードを更新
	for (size_t i = 0;i < node->children.size();i++)
	{
		UpdateActorNode(node->children[i], deltaTime);
	}
}

void DrawActorNode(ActorNode* node)
{
	//末端ノードの場合は終了
	if (node == nullptr) return;

	//自身がルートノードなら自身をDrawで処理
	//(でないならDrawActorNodeで処理)
	if (node->parent != nullptr)
	{
		node->actor->Draw();
	}
	//行きがけ順の再帰処理で子ノードを描画
	for (size_t i = 0;i < node->children.size();i++)
	{
		DrawActorNode(node->children[i]);
	}
}

void ReleaseActorNode(ActorNode* node)
{
	//末端ノードの場合は終了
	if (node == nullptr) return;

	//帰りがけ順の再帰処理で子ノードを解放
	for (size_t i = 0;i < node->children.size();i++)
	{
		ReleaseActorNode(node->children[i]);
	}

	//ノードとノードに割り当てられたActorを解放
	//(ルートノードの場合はActor解放を行わない)
	if (node->parent != nullptr && 
		node->actor  != nullptr)
	{
		delete node->actor;
	}
	delete node;
}

GridHeightMap::GridHeightMap()
	: xVertNum(5)
	, yVertNum(5)
	, minPos(-50.0f, -50.0f)
	, maxPos(50.0f, 50.0f)
	, gridWidth(100.0f / ((float)(xVertNum - 1) - 1.0f), 100.0f / ((float)(yVertNum - 1) - 1.0f))
	, heightMap(xVertNum * yVertNum)
{
	for (int i = 0;i < yVertNum;i++)
	{
		for (int j = 0;j < xVertNum;j++)
		{
			int index = j + i * xVertNum;
			heightMap[index] = XMFLOAT3(minPos.x + gridWidth.x * j,
				minPos.y + gridWidth.y * i, 0.0f);
		}
	}
}

GridHeightMap::GridHeightMap(float* height, int xNum, int yNum, XMFLOAT2 base,
	float xWidth, float yWidth)
	: xVertNum(xNum)
	, yVertNum(yNum)
	, minPos(base)
	, maxPos(base + XMFLOAT2(xWidth, yWidth))
	, gridWidth(xWidth / (float)(xVertNum - 1), yWidth / (float)(yVertNum - 1))
	, heightMap(xNum * yNum)
{
	for (int i = 0;i < yVertNum;i++)
	{
		for (int j = 0;j < xVertNum;j++)
		{
			int index = j + i * xVertNum;
			heightMap[index] = XMFLOAT3(minPos.x + gridWidth.x * j,
				minPos.y + gridWidth.y * i, height[index]);
		}
	}
}

Ray::Ray()
	: pos(ZeroVec3)
	, dir(UnitVec3X)
{
}

Ray::Ray(XMFLOAT3 p, XMFLOAT3 d)
	: pos(p)
	, dir(Normalize(d))
{
}

XMFLOAT3 Ray::CalcPositionOnRay(float rho) const
{
	return pos + rho * dir;
}

SceneData::SceneData()
	:padA(0.0f)
{
	viewProj = XMMatrixIdentity();
	eye		 = ZeroVec3;
}

ColorRGBA::ColorRGBA()
	: r(0)
	, g(0)
	, b(0)
	, a(255)
{
}

ColorRGBA::ColorRGBA(unsigned char red, unsigned char green, unsigned char blue,
	unsigned char alpha)
	: r(red)
	, g(green)
	, b(blue)
	, a(alpha)
{
}

BufferIndex::BufferIndex(int vertexBufferNum, int indexBufferNum,
	int constBufferNum, int imgBufferNum)
	:vertexIndices(vertexBufferNum)
	, indexIndices(indexBufferNum)
	, constIndices(constBufferNum)
	, imgData(imgBufferNum)
{
	//バッファ無しの状態として初期化
	for (int i = 0;i < vertexBufferNum;i++)
	{
		vertexIndices[i] = -1;
	}
	for (int i = 0;i < indexBufferNum;i++)
	{
		indexIndices[i] = -1;
	}
	for (int i = 0;i < constBufferNum;i++)
	{
		constIndices[i] = -1;
	}
	for (int i = 0;i < imgBufferNum;i++)
	{
		imgData[i] = { -1,0,0,0,L"" };
	}
}

MeshMaterial::MeshMaterial()
	: diffuse(OneVec3)
	, padA(0.0f)
	, specular(XMFLOAT3(0.1f, 0.1f, 0.1f))
	, specPower(1.0f)
	, ambient(XMFLOAT3(0.1f, 0.1f, 0.1f))
	, padB(0.0f)
{
}

MeshMaterial::MeshMaterial(XMFLOAT3 diff, XMFLOAT3 spec, float p, XMFLOAT3 amb)
	: diffuse(diff)
	, padA(0.0f)
	, specular(spec)
	, specPower(p)
	, ambient(amb)
	, padB(0.0f)
{
}

ModelData::ModelData()
	: world(XMMatrixIdentity())
{
}

ModelData::ModelData(XMMATRIX w, MeshMaterial material)
	: world(w)
	, material(material)
{
}

AnimModelData::AnimModelData()
	: world(XMMatrixIdentity())
{
	for (int i = 0;i < BONE_MAX_NUM;i++)
	{
		inverseMatrices[i] = XMMatrixIdentity();
		poseMatrices[i]	   = XMMatrixIdentity();
	}
}

AnimModelData::AnimModelData(XMMATRIX w, MeshMaterial mat)
	: world(w)
	, material(mat)
{
	for (int i = 0;i < BONE_MAX_NUM;i++)
	{
		inverseMatrices[i] = XMMatrixIdentity();
		poseMatrices[i]	   = XMMatrixIdentity();
	}
}

void AnimModelData::SetInverseMatrices(const std::vector<XMMATRIX>& matrices)
{
	for (size_t i = 0;i < matrices.size();i++)
	{
		inverseMatrices[i] = matrices[i];
	}
}

void AnimModelData::SetPoseMatrices(const std::vector<XMMATRIX>& matrices)
{
	for (size_t i = 0;i < matrices.size();i++)
	{
		poseMatrices[i] = matrices[i];
	}
}

AABB::AABB()
	: oriMax(0.5f * OneVec3)
	, oriMin(-0.5f * OneVec3)
	, curMax(0.5f * OneVec3)
	, curMin(-0.5f * OneVec3)
{
}

AABB::AABB(std::vector<XMFLOAT3>& points)
	: oriMax(ZeroVec3)
	, oriMin(ZeroVec3)
	, curMax(ZeroVec3)
	, curMin(ZeroVec3)
{
	Initialize(points);
}

void AABB::Initialize(std::vector<XMFLOAT3>& points)
{
	if (points.size() == 0) return;

	//引数点群に基づいてAABBを構築
	//各軸の最大値と最小値を求める
	oriMax = points[0];
	oriMin = points[0];
	for (size_t i = 1;i < points.size();i++)
	{
		if (oriMax.x < points[i].x) oriMax.x = points[i].x;
		if (oriMax.x > points[i].x) oriMax.x = points[i].x;
		if (oriMax.y < points[i].y) oriMax.y = points[i].y;
		if (oriMax.y > points[i].y) oriMax.y = points[i].y;
		if (oriMax.z < points[i].z) oriMax.z = points[i].z;
		if (oriMax.z > points[i].z) oriMax.z = points[i].z;
	}
	curMax = oriMax;
	curMin = oriMin;
}

void AABB::Update(const XMMATRIX& world)
{
	//各軸の最大値と最小値から8点の座標を作成
	std::vector<XMFLOAT3> points(8);
	points[0] = XMFLOAT3(oriMax.x, oriMax.y, oriMax.z);
	points[1] = XMFLOAT3(oriMin.x, oriMax.y, oriMax.z);
	points[2] = XMFLOAT3(oriMax.x, oriMin.y, oriMax.z);
	points[3] = XMFLOAT3(oriMax.x, oriMax.y, oriMin.z);
	points[4] = XMFLOAT3(oriMin.x, oriMin.y, oriMax.z);
	points[5] = XMFLOAT3(oriMin.x, oriMax.y, oriMin.z);
	points[6] = XMFLOAT3(oriMax.x, oriMin.y, oriMin.z);
	points[7] = XMFLOAT3(oriMin.x, oriMin.y, oriMin.z);

	//AABBを所有するActorのワールド行列でAABBの再計算
	curMax = points[0] * world;
	curMin = curMax;
	for (size_t i = 1;i < points.size();i++)
	{
		XMFLOAT3 p = points[i] * world;
		if (curMax.x < p.x) curMax.x = p.x;
		if (curMin.x > p.x) curMin.x = p.x;
		if (curMax.y < p.y) curMax.y = p.y;
		if (curMin.y > p.y) curMin.y = p.y;
		if (curMax.z < p.z) curMax.z = p.z;
		if (curMin.z > p.z) curMin.z = p.z;
	}
}

OBB::OBB()
	: pos(ZeroVec3)
	, dirX(UnitVec3X)
	, dirY(UnitVec3Y)
	, dirZ(UnitVec3Z)
	, scale(OneVec3)
{
}

OBB::OBB(XMFLOAT3 p, XMFLOAT3 dx, XMFLOAT3 dy, XMFLOAT3 dz, XMFLOAT3 s)
	: pos(p)
	, dirX(dx)
	, dirY(dy)
	, dirZ(dz)
	, scale(s)
{
}

XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, float drawW, float drawH, float screenW, float screenH)
{
	XMMATRIX mat = XMMatrixIdentity();
	float fractionW = 1.0f / screenW;
	float fractionH = 1.0f / screenH;
	mat.r[0].m128_f32[0] = 2.0f * drawW * fractionW;
	mat.r[1].m128_f32[1] = 2.0f * drawH * fractionH;
	mat.r[3].m128_f32[0] = (2.0f * p1.x + drawW - screenW) * fractionW;
	mat.r[3].m128_f32[1] = (-2.0f * p1.y - drawH + screenH) * fractionH;

	return mat;
}
XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float screenW, float screenH)
{
	return CalcSpriteMatrix(p1, p2.x - p1.x, p2.y - p1.y, screenW, screenH);
}

XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, float uvW, float uvH, float imgW, float imgH)
{
	XMMATRIX mat = XMMatrixIdentity();
	float fractionW = 1.0f / imgW;
	float fractionH = 1.0f / imgH;
	mat.r[0].m128_f32[0] = uvW * fractionW;
	mat.r[1].m128_f32[1] = uvW * fractionH;
	mat.r[3].m128_f32[0] = p1.x * fractionW;
	mat.r[3].m128_f32[1] = p1.y * fractionH;

	return mat;
}
XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float imgW, float imgH)
{
	return CalcSpriteMatrix(p1, p2.x - p1.x, p2.y - p1.y, imgW, imgH);
}

int CalcFontimageIndex(wchar_t c)
{
	if ((c >= L' ') && (c < L' ' + 96))		   return (int)(c - L' ');
	if ((c >= L'あ') && (c < L'あ' + 190))	   return (int)(96+(c - L'あ'));
	if ((c >= 0x4E00) && (c < 0x4E00 + 20950)) return (int)(96 + 190 + (c - 0x4E00));

	return -1;
}

void ConvertString(std::string& src, std::wstring& dst)
{
	wchar_t* buff = new wchar_t[src.size() + 1];
	size_t cvt;
	mbstowcs_s(&cvt, buff, src.size() + 1, src.c_str(), _TRUNCATE);
	dst = buff;
	delete[] buff;
}

void SetContainerToMatrix(const std::vector<float>& v, XMMATRIX& mat)
{
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			mat.r[i].m128_f32[j] = v[j + i * 4];
		}
	}
}

void SetContainerToMatrix(const std::vector<float>& v, std::vector<XMMATRIX>& mat)
{
	int index = 0;
	for (size_t i = 0;i < mat.size();i++)
	{
		for (int y = 0;y < 4;y++)
		{
			for (int x = 0;x < 4;x++)
			{
				mat[i].r[y].m128_f32[x] = v[index];
				index++;
			}
		}
	}
}

void SetContainerToMatrix(const std::vector<float>& v, std::vector<std::vector<XMMATRIX>>& mat)
{
	int index = 0;
	for (size_t i = 0;i < mat.size();i++)
	{
		for (size_t j = 0;j < mat[i].size();j++)
		{
			for (int y = 0;y < 4;y++)
			{
				for (int x = 0;x < 4;x++)
				{
					mat[i][j].r[y].m128_f32[x] = v[index];
					index++;
				}
			}
		}
	}
}