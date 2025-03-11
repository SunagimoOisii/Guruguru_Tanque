#include "FileAnimMesh.h"
#include "Actor3D.h"
#include "Renderer.h"
#include "GameUtil.h"
#include <fstream>

FileAnimMesh::FileAnimMesh(Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* filePath,
	const wchar_t* imgFilePrefix, bool ddsFlag)
	: Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 1)
	, m_adjustMat(XMMatrixIdentity())
	, m_animIndex(0)
{
	//�Ǎ����b�V���f�[�^�ɍ��킹���ϐ����������s��
	std::vector<AnimMeshData> mData;
	if (!ReadFbAnimFile(filePath, imgFilePrefix, mData)) return;
	size_t meshNum = mData.size();
	m_bufferIndex.vertexIndices.resize(meshNum);
	m_bufferIndex.indexIndices.resize(meshNum);
	m_bufferIndex.constIndices.resize(meshNum);
	m_bufferIndex.imgData.resize(meshNum);
	m_indicesNum.resize(meshNum);
	m_animData.resize(meshNum);
	m_animInterval = mData[0].animData.interval;
	m_animFrameNum = mData[0].animData.frameNum;
	m_poseMatrices.resize(meshNum);
	for (size_t i = 0;i < meshNum;i++)
	{
		m_poseMatrices[i] = mData[i].poseMatrices;
	}

	//���b�V���f�[�^����
	for (size_t i = 0;i < meshNum;i++)
	{
		//�t�o�C���h�|�[�Y�s��ƌ��|�[�Y�s����擾
		m_poseMatrices[i] = mData[i].poseMatrices;
		m_animData[i].SetInverseMatrices(mData[i].inverseGPoseMatrices);
		m_animData[i].SetPoseMatrices(mData[i].poseMatrices[m_animIndex]);

		//���_�o�b�t�@����
		m_bufferIndex.vertexIndices[i] = m_renderer->AllocateVertexBuffer(
			(void*)mData[i].vertices.data(),
			mData[i].vertices.size() * sizeof(VertexAnim), sizeof(VertexAnim));

		//�C���f�b�N�X�o�b�t�@����
		m_bufferIndex.indexIndices[i] = m_renderer->AllocateIndexBuffer(
			(void*)mData[i].indices.data(),
			mData[i].indices.size() * sizeof(unsigned short));
		m_indicesNum[i] = (int)mData[i].indices.size();

		//�萔�o�b�t�@����
		m_bufferIndex.constIndices[i] = m_renderer->AllocateConstBuffer(
			(void*)&m_animData[i], sizeof(AnimModelData));

		//�e�N�X�`���f�[�^����
		m_bufferIndex.imgData[i] = m_renderer->AllocateShaderResource(
			mData[i].textureFileName, ddsFlag);

		//�o�b�t�@�ɂP�ł��s��������΃o�b�t�@���
		if (m_bufferIndex.vertexIndices[i]	  == -1 ||
			m_bufferIndex.indexIndices[i]	  == -1 ||
			m_bufferIndex.constIndices[i]	  == -1 ||
			m_bufferIndex.imgData[i].imgIndex == -1)
		{
			ReleaseBuffer();
			return;
		}
	}
	m_enabled = true;
}

FileAnimMesh::~FileAnimMesh()
{
}

void FileAnimMesh::SetAnimIndex(UINT index)
{
	if (index < 0 || index >= m_animFrameNum) return;
	if (m_animIndex == index) return;

	for (size_t i = 0;i < m_animData.size();i++)
	{
		m_animData[i].SetPoseMatrices(m_poseMatrices[i][index]);
	}
	m_animIndex = index;
}

void FileAnimMesh::Draw()
{
	if (!m_enabled) return;

	m_renderer->SetShader(m_shaderClassIndex);

	for (size_t i = 0;i < m_bufferIndex.vertexIndices.size();i++)
	{
		m_animData[i].SetWorld(m_adjustMat * m_owner->GetWorldMatrix());
		m_animData[i].SetMaterial(m_curMaterial);

		//���b�V�����Ƃɋt�o�C���h�|�[�Y�s��ƌ��|�[�Y�s�񂪈قȂ�̂�
		//�萔�o�b�t�@��]������O�ɂ����ōĐݒ�
		m_renderer->UploadConstBuffer(m_bufferIndex.constIndices[i],
			(void*)&m_animData[i], sizeof(AnimModelData));

		m_renderer->SetVertexBufferViewToCommandList(m_bufferIndex.vertexIndices[i]);
		m_renderer->SetIndexBufferViewToCommandList(m_bufferIndex.indexIndices[i]);
		m_renderer->SetConstBufferViewToCommandList(1, m_bufferIndex.constIndices[i]);
		m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[i].imgIndex);
		m_renderer->DrawMeshIndexed(m_indicesNum[i], 0);
	}
}

//*************************************************************
//��public ��protected
//*************************************************************

bool FileAnimMesh::ReadFbAnimFile(const wchar_t* filePath, const wchar_t* imgFilePrefix,
	std::vector<AnimMeshData>& mData)
{
	mData.clear();
	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	//���b�V���̌��̎擾
	unsigned int meshNum = 0;
	fin.read((char*)&meshNum, sizeof(unsigned int));
	if (fin.gcount() != sizeof(unsigned int)) return false;
	mData.resize(meshNum);

	//���b�V���f�[�^�̎擾
	for (unsigned int i = 0;i < meshNum;i++)
	{
		//���_���̎擾
		unsigned int vertexNum = 0;
		fin.read((char*)&vertexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;

		//�C���f�b�N�X���̎擾
		unsigned int indexNum = 0;
		fin.read((char*)&indexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;

		//���_�f�[�^�̎擾
		mData[i].vertices.resize(vertexNum);
		fin.read((char*)mData[i].vertices.data(), sizeof(VertexAnim) * vertexNum);
		if (fin.gcount() != sizeof(VertexAnim) * vertexNum) return false;

		//�C���f�b�N�X�f�[�^�̎擾
		mData[i].indices.resize(indexNum);
		fin.read((char*)mData[i].indices.data(), sizeof(unsigned short) * indexNum);
		if (fin.gcount() != sizeof(unsigned short) * indexNum) return false;

		//AnimData�\���̂̎擾
		fin.read((char*)&mData[i].animData, sizeof(AnimData));
		if (fin.gcount() != sizeof(AnimData)) return false;

		//�t�o�C���h�|�[�Y�s��̎擾
		int matElementNum = 16 * mData[i].animData.boneNum;
		std::vector<float> inverseMatElements(matElementNum);
		fin.read((char*)inverseMatElements.data(), sizeof(float) * matElementNum);
		if (fin.gcount() != sizeof(float) * matElementNum) return false;
		mData[i].inverseGPoseMatrices.resize(mData[i].animData.boneNum);
		SetContainerToMatrix(inverseMatElements, mData[i].inverseGPoseMatrices);

		//���|�[�Y�s��̎擾
		matElementNum *= mData[i].animData.frameNum;
		std::vector<float> poseMatElements(matElementNum);
		fin.read((char*)poseMatElements.data(), sizeof(float) * matElementNum);
		if (fin.gcount() != sizeof(float) * matElementNum) return false;
		mData[i].poseMatrices.resize(mData[i].animData.frameNum);
		for (int j = 0;j < mData[i].animData.frameNum;j++)
		{
			mData[i].poseMatrices[j].resize(mData[i].animData.boneNum);
		}
		SetContainerToMatrix(poseMatElements, mData[i].poseMatrices);

		//�e�N�X�`���t�@�C�����̎擾
		wchar_t textureName[256]{};
		fin.read((char*)textureName, sizeof(wchar_t) * 256);
		if (fin.gcount() != sizeof(wchar_t) * 256) return false;
		mData[i].textureFileName = imgFilePrefix + std::wstring(textureName);
	}

	//�����p�ϊ��s��̎擾
	std::vector<float> adjustMatElements(16);
	fin.read((char*)adjustMatElements.data(), sizeof(float) * 16);
	if (fin.gcount() != sizeof(float) * 16) return false;
	SetContainerToMatrix(adjustMatElements, m_adjustMat);

	fin.close();
	return true;
}