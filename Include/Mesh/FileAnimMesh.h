#pragma once

#include "Mesh.h"

/// <summary>
/// FbAnim�t�@�C��������
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
�A�j���[�V�����֘A�̕ϐ�
**********************************/

	std::vector<AnimModelData> m_animData; //�萔�o�b�t�@�ɑ��M���郂�f���f�[�^
	UINT  m_animIndex; //���|�[�Y�s��̃A�j���[�V�����t���[���̃C���f�b�N�X
	float m_animInterval; 
	unsigned short m_animFrameNum;

	//�P�A�j���[�V�����ɕK�v�Ȃ͓̂񎟌��z��ɂ����񂾂�
	//�������b�V���ɑΉ����邽�߂ɎO�����z��ɐݒ�
	std::vector<std::vector<std::vector<XMMATRIX>>> m_poseMatrices;
};
