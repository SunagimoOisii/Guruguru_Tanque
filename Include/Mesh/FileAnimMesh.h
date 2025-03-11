#pragma once

#include "Mesh.h"

/// <summary>
/// FbAnimファイルを扱う
/// </summary>
class FileAnimMesh : public Mesh
{
public:
	FileAnimMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* filePath,
		const wchar_t* imgFilePrefix = L"image\\", bool ddsFlag = false);
	~FileAnimMesh();

	UINT  GetAnimIndex() const { return m_animIndex; }
	void  SetAnimIndex(UINT index);
	float GetInterval() const { return m_animInterval; }
	unsigned short GetFrameNum() const { return m_animFrameNum; }

	void Draw() override;

protected:
	bool ReadFbAnimFile(const wchar_t* filePath, const wchar_t* imgFilePrefix,
		std::vector<AnimMeshData>& mData);

	std::vector<int> m_indicesNum;
	XMMATRIX m_adjustMat;

/**********************************
アニメーション関連の変数
**********************************/

	std::vector<AnimModelData> m_animData; //定数バッファに送信するモデルデータ
	UINT  m_animIndex; //現ポーズ行列のアニメーションフレームのインデックス
	float m_animInterval; 
	unsigned short m_animFrameNum;

	//１アニメーションに必要なのは二次元配列にある情報だが
	//複数メッシュに対応するために三次元配列に設定
	std::vector<std::vector<std::vector<XMMATRIX>>> m_poseMatrices;
};
