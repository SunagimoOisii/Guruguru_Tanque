#pragma once

#include <vector>
#include <string>
#include <xaudio2.h>

#include "Math.h"

//単色テクスチャの名前宣言

extern const wchar_t* WHITE_TEX;
extern const wchar_t* GRAY_TEX;
extern const wchar_t* BLACK_TEX;
extern const wchar_t* RED_TEX;
extern const wchar_t* GREEN_TEX;
extern const wchar_t* BLUE_TEX;
extern const wchar_t* YELLOW_TEX;
extern const wchar_t* CYAN_TEX;
extern const wchar_t* MAGENTA_TEX;

//本ファイルの構造体でのpadA,B…はパディングのためにある(CPUとGPU間のアライメント間隔の違いを是正)
/**********************************
ライト構造体
**********************************/

namespace Light
{
	struct Parallel
	{
		XMFLOAT3 direction;
		float	 padA;
		XMFLOAT3 color;
		float	 padB;

		Parallel();
		Parallel(XMFLOAT3 dir, XMFLOAT3 col);
	};

	struct Point
	{
		XMFLOAT3 pos;
		float	 pad;
		XMFLOAT3 color;
		float	 radius; //減衰半径

		Point();
		Point(XMFLOAT3 p, XMFLOAT3 col, float r);
	};

	struct Spot
	{
		XMFLOAT3 pos;
		float	 radius;
		XMFLOAT3 direction;
		float    radiationAngle;
		XMFLOAT3 color;
		float	 pad;

		Spot();
		Spot(XMFLOAT3 p, float r, XMFLOAT3 dir, float angle, XMFLOAT3 col);
	};

	struct Ambient
	{
		XMFLOAT3 color;
		float	 pad;

		Ambient();
		Ambient(XMFLOAT3 col);
	};
}

/**********************************
図形構造体
**********************************/

namespace Shape
{
	struct Segment
	{
		XMFLOAT2 start;
		XMFLOAT2 end;

		Segment();
		Segment(const XMFLOAT2& a, const XMFLOAT2& b);
	};

	struct Circle
	{
		XMFLOAT2 pos;
		float r;

		Circle();
		Circle(const XMFLOAT2& p, float rad);
	};

	struct Rect
	{
		float left;
		float top;
		float right;
		float bottom;

		Rect();
		Rect(float lt, float tp, float rt, float bm);
	};

	struct RotateRect
	{
		XMFLOAT2 pos;
		float width;
		float height;
		float angle;

		RotateRect();
		RotateRect(XMFLOAT2 pos, float width, float height,float angle);
	};

	struct Sphere
	{
		XMFLOAT3 pos;
		float r;

		Sphere();
		Sphere(const XMFLOAT3& p, float radius);
	};
	
	struct InfinitePlane
	{
		XMFLOAT3 normal;
		float distance;

		InfinitePlane();
		InfinitePlane(XMFLOAT3 nor, float dist);
		InfinitePlane(XMFLOAT3 nor, XMFLOAT3 pos);
	};

	struct FinitePlane
	{
		XMFLOAT3 pos[4];

		FinitePlane();
		FinitePlane(XMFLOAT3* p);
		FinitePlane(const XMFLOAT3& cPos,
			const XMFLOAT3& xVec, const XMFLOAT3& yVec, float scaleX, float scaleY);
	};
}

struct ActorNode
{
	ActorNode* parent;
	std::vector<ActorNode*> children;
	class Actor3D* actor;

	ActorNode();
	ActorNode(class Actor3D* actor);
	
	void AddNode(ActorNode* node);
};
void UpdateActorNode(ActorNode* node, float deltaTime);
void DrawActorNode(ActorNode* node);
void ReleaseActorNode(ActorNode* node);

/// <summary>
/// GridMesh生成に必要な情報を持った構造体
/// </summary>
/// <param name = "x,yVertNum">格子の各方向の頂点数</param>
/// <param name = "min,maxPos">XY平面上の範囲</param>
/// <param name = "gridWidth">格子の各方向の幅</param>
/// <param name = "heightMap">格子の頂点情報</param>
struct GridHeightMap
{
	int xVertNum;
	int yVertNum;
	XMFLOAT2 minPos;
	XMFLOAT2 maxPos;
	XMFLOAT2 gridWidth;
	std::vector<XMFLOAT3> heightMap;

	GridHeightMap();
	GridHeightMap(float* height, int xNum, int yNum, XMFLOAT2 base,
		float xWidth, float yWidth);
};

struct Ray
{
	XMFLOAT3 pos;
	XMFLOAT3 dir;

	Ray();
	Ray(XMFLOAT3 p, XMFLOAT3 d);
	XMFLOAT3 CalcPositionOnRay(float rho) const;
};

/// <summary>
/// シーン共通のデータ用の構造体
/// </summary>
struct SceneData
{
	XMMATRIX viewProj;
	XMFLOAT3 eye;
	float padA;
	Light::Parallel parallelLight;
	Light::Point	pointLights[10];
	Light::Spot	    spotLights[3];
	Light::Ambient  ambientLight;

	SceneData();
};

struct ColorRGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	ColorRGBA();
	ColorRGBA(unsigned char red, unsigned char green, unsigned char blue,
		unsigned char alpha);
};

struct ImageData
{
	int imgIndex = -1;
	UINT width   = 0;
	UINT height  = 0;
	int counter  = 0;
	std::wstring filePath = L"";
};

/// <summary>
///Rendererで管理するバッファのインデックス情報をまとめる
/// </summary>
struct BufferIndex
{
	std::vector<int>	   vertexIndices;
	std::vector<int>	   indexIndices;
	std::vector<int>	   constIndices;
	std::vector<ImageData> imgData;

	BufferIndex(int vertexBufferNum, int indexBufferNum,
		int constBufferNum, int imgBufferNum);
};

struct SoundData
{
	WAVEFORMATEX format;
	std::vector<BYTE> wavData;
};

struct Transformat
{
	XMMATRIX posMat;
	XMMATRIX uvMat;
};

/// <summary>
/// 法線対応の頂点構造体
/// </summary>
struct VertexNormal
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

//頂点に影響を及ぼすボーンの最大数
#define ANIM_BONE_MAX_NUM (4)
struct VertexAnim //アニメーション情報に対応した頂点の型
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	unsigned short boneID[ANIM_BONE_MAX_NUM]; //影響するボーンのインデックス
	float weight[ANIM_BONE_MAX_NUM]; //ボーンの影響度
};

struct MeshData
{
	std::vector<struct VertexNormal> vertices;
	std::vector<unsigned short> indices;
	std::wstring textureFileName;
};

struct AnimData
{
	unsigned short boneNum;
	unsigned short frameNum;
	float interval;
};

/// <summary>
/// アニメーションメッシュの構造体<para></para>
/// FbAnimファイル読込に用いる
/// </summary>
struct AnimMeshData
{
	std::vector<VertexAnim> vertices;
	std::vector<unsigned short> indices;
	AnimData animData;
	std::vector<XMMATRIX> inverseGPoseMatrices;
	std::vector<std::vector<XMMATRIX>> poseMatrices;
	std::wstring textureFileName;
};

/// <summary>
/// 拡散反射光,鏡面反射光,環境光の反射係数と鏡面反射の絞り(乗数)を保持
/// </summary>
struct MeshMaterial
{
	XMFLOAT3 diffuse;
	float padA;
	XMFLOAT3 specular;
	float specPower;
	XMFLOAT3 ambient;
	float padB;

	MeshMaterial();
	MeshMaterial(XMFLOAT3 diff, XMFLOAT3 spec, float p, XMFLOAT3 amb);
};

/// <summary>
/// モデル用定数バッファに送信するデータ構造体
/// </summary>
struct ModelData
{
	XMMATRIX world;
	MeshMaterial material;

	ModelData();
	ModelData(XMMATRIX w, MeshMaterial material);
};

#define BONE_MAX_NUM (256)
struct AnimModelData
{
	XMMATRIX world;
	MeshMaterial material;
	XMMATRIX inverseMatrices[BONE_MAX_NUM];
	XMMATRIX poseMatrices[BONE_MAX_NUM];

	AnimModelData();
	AnimModelData(XMMATRIX w, MeshMaterial mat);
	void SetWorld(XMMATRIX w) { world = w; }
	void SetMaterial(MeshMaterial mat) { material = mat; }
	void SetInverseMatrices(const std::vector<XMMATRIX>& matrices);
	void SetPoseMatrices(const std::vector<XMMATRIX>& matrices);
};

struct AABB
{
	XMFLOAT3 oriMax;
	XMFLOAT3 oriMin;
	XMFLOAT3 curMax;
	XMFLOAT3 curMin;

	AABB();
	AABB(std::vector<XMFLOAT3>& points);

	void Initialize(std::vector<XMFLOAT3>& points);
	void Update(const XMMATRIX& world);
};

struct OBB
{
	XMFLOAT3 pos;
	XMFLOAT3 dirX;
	XMFLOAT3 dirY;
	XMFLOAT3 dirZ;
	XMFLOAT3 scale;

	OBB();
	OBB(XMFLOAT3 p, XMFLOAT3 dx, XMFLOAT3 dy, XMFLOAT3 dz, XMFLOAT3 s);
};

/// <summary>
/// スプライトの座標変換行列を返す
/// </summary>
///  <param name="p1">左上座標</param>
///  <param name="drawW,H">描画サイズ</param>
///  <param name="screenW,H">スクリーンサイズ</param>
XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, float drawW, float drawH, float screenW, float screenH);

/// <summary>
/// スプライトの座標変換行列を返す
/// </summary>
///  <param name="p1,p2">左上,右下座標</param>
///  <param name="drawW,drawH">描画サイズ</param>
///  <param name="screenW,screenH">スクリーンサイズ</param>
XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float screenW, float screenH);

/// <summary>
/// スプライトのUV座標変換行列を返す
/// </summary>
/// <param name="p1">左上座標</param>
/// <param name="uvW,H">UVサイズ</param>
/// <param name="imgW,H">テクスチャサイズ</param>
XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, float uvW, float uvH, float imgW, float imgH);

/// <summary>
/// スプライトのUV座標変換行列を返す
/// </summary>
/// <param name="p1,p2">左上,右下座標</param>
/// <param name="imgW,H">テクスチャサイズ</param>
XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float imgW, float imgH);

int CalcFontimageIndex(wchar_t c);

/// <summary>
///ワイド文字列への変換関数
/// </summary>
void ConvertString(std::string& src, std::wstring& dst);

/// <summary>
/// vectorコンテナからXMMATRIXに内容をコピー<para></para>
/// FbFixファイル読込に使用
/// </summary>
/// <param name="v">要素が16個あることが前提</param>
void SetContainerToMatrix(const std::vector<float>& v, XMMATRIX& mat);

/// <summary>
/// vectorコンテナからXMMATRIX配列に内容をコピー<para></para>
/// FbAnimファイルの逆バインドポーズ行列読込に使用
/// </summary>
/// <param name="v">要素が16個あることが前提</param>
void SetContainerToMatrix(const std::vector<float>& v, std::vector<XMMATRIX>& mat);

/// <summary>
/// vectorコンテナからXMMATRIX二次元配列に内容をコピー<para></para>
/// FbAnimファイルの現ポーズ行列読込に使用
/// </summary>
/// <param name="v">要素が16個あることが前提</param>
void SetContainerToMatrix(const std::vector<float>& v, std::vector<std::vector<XMMATRIX>>& mat);