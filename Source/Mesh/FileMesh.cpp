#include "FileMesh.h"
#include "Actor3D.h"
#include "Renderer.h"
#include "GameUtil.h"
#include <fstream>

FileMesh::FileMesh(class Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* filePath,
	const wchar_t* imgFilePrefix, bool ddsFlag)
	: Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 1)
	, m_adjustMat(XMMatrixIdentity())
{
	std::vector<MeshData> mData;
	if (!ReadFbfixFile(filePath, imgFilePrefix, mData)) return;

	size_t meshNum = mData.size();
	m_bufferIndex.vertexIndices.resize(meshNum);
	m_bufferIndex.indexIndices.resize(meshNum);
	m_bufferIndex.imgData.resize(meshNum);
	m_indicesNum.resize(meshNum);

	//�萔�o�b�t�@����
	ModelData modelData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&modelData, sizeof(ModelData));
	if (m_bufferIndex.constIndices[0] == -1)
	{
		ReleaseBuffer();
		return;
	}

	//���b�V���f�[�^����(���_,�C���f�b�N�X,�e�N�X�`��)
	for (size_t i = 0;i < meshNum;i++)
	{
		//���_�o�b�t�@����
		m_bufferIndex.vertexIndices[i] = m_renderer->AllocateVertexBuffer(
			(void*)mData[i].vertices.data(),
			mData[i].vertices.size() * sizeof(VertexNormal), sizeof(VertexNormal));

		//�C���f�b�N�X�o�b�t�@����
		m_bufferIndex.indexIndices[i] = m_renderer->AllocateIndexBuffer(
			(void*)mData[i].indices.data(),
			mData[i].indices.size() * sizeof(unsigned short));
		m_indicesNum[i] = (int)mData[i].indices.size();

		//�e�N�X�`���f�[�^����
		m_bufferIndex.imgData[i] = m_renderer->AllocateShaderResource(
			mData[i].textureFileName, ddsFlag);

		//�ǂꂩ�P�ł��s��������΃o�b�t�@���
		if (m_bufferIndex.vertexIndices[i] == -1 ||
			m_bufferIndex.indexIndices[i] == -1 ||
			m_bufferIndex.imgData[i].imgIndex == -1)
		{
			ReleaseBuffer();
			return;
		}
	}
	m_enabled = true;
}

FileMesh::~FileMesh()
{
}

void FileMesh::Draw()
{
	if (!m_enabled) return;

	//�萔�o�b�t�@�ɑ��郏�[���h�s��ɒ����p�ϊ��s���������
	ModelData mData(m_adjustMat * m_owner->GetWorldMatrix(), m_curMaterial);
	m_renderer->UploadConstBuffer(m_bufferIndex.constIndices[0],
		(void*)&mData, sizeof(ModelData));

	m_renderer->SetShader(m_shaderClassIndex);
	m_renderer->SetConstBufferViewToCommandList(1, m_bufferIndex.constIndices[0]);

	for (size_t i = 0;i < m_bufferIndex.vertexIndices.size();i++)
	{
		m_renderer->SetVertexBufferViewToCommandList(m_bufferIndex.vertexIndices[i]);
		m_renderer->SetIndexBufferViewToCommandList(m_bufferIndex.indexIndices[i]);
		m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[i].imgIndex);
		m_renderer->DrawMeshIndexed(m_indicesNum[i], 0);
	}
}

//*************************************************************
//��public ��private
//*************************************************************

bool FileMesh::ReadFbfixFile(const wchar_t* filePath, const wchar_t* imgFilePrefix,
	std::vector<MeshData>& mData)
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
		fin.read((char*)mData[i].vertices.data(), sizeof(VertexNormal) * vertexNum);
		if (fin.gcount() != sizeof(VertexNormal) * vertexNum) return false;

		//�C���f�b�N�X�f�[�^�̎擾
		mData[i].indices.resize(indexNum);
		fin.read((char*)mData[i].indices.data(), sizeof(unsigned short) * indexNum);
		if (fin.gcount() != sizeof(unsigned short) * indexNum) return false;

		//�e�N�X�`���t�@�C�����̎擾
		wchar_t textureName[256]{};
		fin.read((char*)textureName, sizeof(wchar_t) * 256);
		if (fin.gcount() != sizeof(wchar_t) * 256) return false;
		mData[i].textureFileName = imgFilePrefix + std::wstring(textureName);
	}

	//�����p�ϊ��s��̎擾
	std::vector<float> adjust(16);
	fin.read((char*)adjust.data(), sizeof(float) * 16);
	if (fin.gcount() != sizeof(float) * 16)
	{
		return false;
	}
	SetContainerToMatrix(adjust, m_adjustMat);

	fin.close();
	return true;
}