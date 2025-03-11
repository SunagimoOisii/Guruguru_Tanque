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

	//定数バッファ生成
	ModelData modelData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&modelData, sizeof(ModelData));
	if (m_bufferIndex.constIndices[0] == -1)
	{
		ReleaseBuffer();
		return;
	}

	//メッシュデータ生成(頂点,インデックス,テクスチャ)
	for (size_t i = 0;i < meshNum;i++)
	{
		//頂点バッファ生成
		m_bufferIndex.vertexIndices[i] = m_renderer->AllocateVertexBuffer(
			(void*)mData[i].vertices.data(),
			mData[i].vertices.size() * sizeof(VertexNormal), sizeof(VertexNormal));

		//インデックスバッファ生成
		m_bufferIndex.indexIndices[i] = m_renderer->AllocateIndexBuffer(
			(void*)mData[i].indices.data(),
			mData[i].indices.size() * sizeof(unsigned short));
		m_indicesNum[i] = (int)mData[i].indices.size();

		//テクスチャデータ生成
		m_bufferIndex.imgData[i] = m_renderer->AllocateShaderResource(
			mData[i].textureFileName, ddsFlag);

		//どれか１つでも不備があればバッファ解放
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

	//定数バッファに送るワールド行列に調整用変換行列をかける
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
//↑public ↓private
//*************************************************************

bool FileMesh::ReadFbfixFile(const wchar_t* filePath, const wchar_t* imgFilePrefix,
	std::vector<MeshData>& mData)
{
	mData.clear();
	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	//メッシュの個数の取得
	unsigned int meshNum = 0;
	fin.read((char*)&meshNum, sizeof(unsigned int));
	if (fin.gcount() != sizeof(unsigned int)) return false;
	mData.resize(meshNum);

	//メッシュデータの取得
	for (unsigned int i = 0;i < meshNum;i++)
	{
		//頂点個数の取得
		unsigned int vertexNum = 0;
		fin.read((char*)&vertexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;

		//インデックス個数の取得
		unsigned int indexNum = 0;
		fin.read((char*)&indexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;

		//頂点データの取得
		mData[i].vertices.resize(vertexNum);
		fin.read((char*)mData[i].vertices.data(), sizeof(VertexNormal) * vertexNum);
		if (fin.gcount() != sizeof(VertexNormal) * vertexNum) return false;

		//インデックスデータの取得
		mData[i].indices.resize(indexNum);
		fin.read((char*)mData[i].indices.data(), sizeof(unsigned short) * indexNum);
		if (fin.gcount() != sizeof(unsigned short) * indexNum) return false;

		//テクスチャファイル名の取得
		wchar_t textureName[256]{};
		fin.read((char*)textureName, sizeof(wchar_t) * 256);
		if (fin.gcount() != sizeof(wchar_t) * 256) return false;
		mData[i].textureFileName = imgFilePrefix + std::wstring(textureName);
	}

	//調整用変換行列の取得
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