#pragma once

#include <vector>
#include <string>
#include <xaudio2.h>

#include "Math.h"

//�P�F�e�N�X�`���̖��O�錾

extern const wchar_t* WHITE_TEX;
extern const wchar_t* GRAY_TEX;
extern const wchar_t* BLACK_TEX;
extern const wchar_t* RED_TEX;
extern const wchar_t* GREEN_TEX;
extern const wchar_t* BLUE_TEX;
extern const wchar_t* YELLOW_TEX;
extern const wchar_t* CYAN_TEX;
extern const wchar_t* MAGENTA_TEX;

//�{�t�@�C���̍\���̂ł�padA,B�c�̓p�f�B���O�̂��߂ɂ���(CPU��GPU�Ԃ̃A���C�����g�Ԋu�̈Ⴂ�𐥐�)
/**********************************
���C�g�\����
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
		float	 radius; //�������a

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
�}�`�\����
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
/// GridMesh�����ɕK�v�ȏ����������\����
/// </summary>
/// <param name = "x,yVertNum">�i�q�̊e�����̒��_��</param>
/// <param name = "min,maxPos">XY���ʏ�͈̔�</param>
/// <param name = "gridWidth">�i�q�̊e�����̕�</param>
/// <param name = "heightMap">�i�q�̒��_���</param>
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
/// �V�[�����ʂ̃f�[�^�p�̍\����
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
///Renderer�ŊǗ�����o�b�t�@�̃C���f�b�N�X�����܂Ƃ߂�
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
/// �@���Ή��̒��_�\����
/// </summary>
struct VertexNormal
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
};

//���_�ɉe�����y�ڂ��{�[���̍ő吔
#define ANIM_BONE_MAX_NUM (4)
struct VertexAnim //�A�j���[�V�������ɑΉ��������_�̌^
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 normal;
	unsigned short boneID[ANIM_BONE_MAX_NUM]; //�e������{�[���̃C���f�b�N�X
	float weight[ANIM_BONE_MAX_NUM]; //�{�[���̉e���x
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
/// �A�j���[�V�������b�V���̍\����<para></para>
/// FbAnim�t�@�C���Ǎ��ɗp����
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
/// �g�U���ˌ�,���ʔ��ˌ�,�����̔��ˌW���Ƌ��ʔ��˂̍i��(�搔)��ێ�
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
/// ���f���p�萔�o�b�t�@�ɑ��M����f�[�^�\����
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
/// �X�v���C�g�̍��W�ϊ��s���Ԃ�
/// </summary>
///  <param name="p1">������W</param>
///  <param name="drawW,H">�`��T�C�Y</param>
///  <param name="screenW,H">�X�N���[���T�C�Y</param>
XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, float drawW, float drawH, float screenW, float screenH);

/// <summary>
/// �X�v���C�g�̍��W�ϊ��s���Ԃ�
/// </summary>
///  <param name="p1,p2">����,�E�����W</param>
///  <param name="drawW,drawH">�`��T�C�Y</param>
///  <param name="screenW,screenH">�X�N���[���T�C�Y</param>
XMMATRIX CalcSpriteMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float screenW, float screenH);

/// <summary>
/// �X�v���C�g��UV���W�ϊ��s���Ԃ�
/// </summary>
/// <param name="p1">������W</param>
/// <param name="uvW,H">UV�T�C�Y</param>
/// <param name="imgW,H">�e�N�X�`���T�C�Y</param>
XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, float uvW, float uvH, float imgW, float imgH);

/// <summary>
/// �X�v���C�g��UV���W�ϊ��s���Ԃ�
/// </summary>
/// <param name="p1,p2">����,�E�����W</param>
/// <param name="imgW,H">�e�N�X�`���T�C�Y</param>
XMMATRIX CalcSpriteUVMatrix(const XMFLOAT2& p1, const XMFLOAT2& p2, float imgW, float imgH);

int CalcFontimageIndex(wchar_t c);

/// <summary>
///���C�h������ւ̕ϊ��֐�
/// </summary>
void ConvertString(std::string& src, std::wstring& dst);

/// <summary>
/// vector�R���e�i����XMMATRIX�ɓ��e���R�s�[<para></para>
/// FbFix�t�@�C���Ǎ��Ɏg�p
/// </summary>
/// <param name="v">�v�f��16���邱�Ƃ��O��</param>
void SetContainerToMatrix(const std::vector<float>& v, XMMATRIX& mat);

/// <summary>
/// vector�R���e�i����XMMATRIX�z��ɓ��e���R�s�[<para></para>
/// FbAnim�t�@�C���̋t�o�C���h�|�[�Y�s��Ǎ��Ɏg�p
/// </summary>
/// <param name="v">�v�f��16���邱�Ƃ��O��</param>
void SetContainerToMatrix(const std::vector<float>& v, std::vector<XMMATRIX>& mat);

/// <summary>
/// vector�R���e�i����XMMATRIX�񎟌��z��ɓ��e���R�s�[<para></para>
/// FbAnim�t�@�C���̌��|�[�Y�s��Ǎ��Ɏg�p
/// </summary>
/// <param name="v">�v�f��16���邱�Ƃ��O��</param>
void SetContainerToMatrix(const std::vector<float>& v, std::vector<std::vector<XMMATRIX>>& mat);