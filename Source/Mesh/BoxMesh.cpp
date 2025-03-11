#include "BoxMesh.h"
#include "Actor3D.h"
#include "Renderer.h"

BoxMesh::BoxMesh(Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, std::vector<const wchar_t*>& texturePath,
	bool outer, float scaleX, float scaleY, float scaleZ,
	bool ddsFlag)
	:Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 6)
{
	if (texturePath.size() != 1 && texturePath.size() != 6) return;
	if (texturePath.size() == 1) m_bufferIndex.imgData.resize(1);

//各種バッファの生成
	//頂点バッファ
	XMFLOAT2 uv[4];
	uv[0] = { 0.0f,1.0f };
	uv[1] = { 0.0f,0.0f };
	uv[2] = { 1.0f,1.0f };
	uv[3] = { 1.0f,0.0f };

	//頂点データの設定
	VertexNormal vertices[24]{};
	float sign = (outer) ? 1.0f : -1.0f;
	//x_front
	XMFLOAT3 norm = outer * UnitVec3X;
	vertices[0] = { { 0.5f * scaleX , sign * 0.5f * scaleY  , -0.5f * scaleZ},uv[0],norm };
	vertices[1] = { { 0.5f * scaleX , sign * 0.5f * scaleY  ,  0.5f * scaleZ}, uv[1],norm };
	vertices[2] = { { 0.5f * scaleX , sign * -0.5f * scaleY ,  -0.5f * scaleZ},uv[2],norm };
	vertices[3] = { { 0.5f * scaleX , sign * -0.5f * scaleY ,   0.5f * scaleZ}, uv[3],norm };
	//y_front
	norm = outer * UnitVec3Y;
	vertices[4] = { { sign * -0.5f * scaleX ,0.5f * scaleY , -0.5f * scaleZ},uv[0],norm };
	vertices[5] = { { sign * -0.5f * scaleX ,0.5f * scaleY ,  0.5f * scaleZ},uv[1],norm };
	vertices[6] = { { sign *  0.5f * scaleX ,0.5f * scaleY , -0.5f * scaleZ},uv[2],norm };
	vertices[7] = { { sign *  0.5f * scaleX ,0.5f * scaleY ,  0.5f * scaleZ},uv[3],norm };
	//x_back
	norm = -1.0f * outer * UnitVec3X;
	vertices[8]  = { { -0.5f * scaleX , sign * -0.5f * scaleY , -0.5f * scaleZ},uv[0],norm };
	vertices[9]  = { { -0.5f * scaleX , sign * -0.5f * scaleY ,  0.5f * scaleZ},uv[1],norm };
	vertices[10] = { { -0.5f * scaleX , sign *  0.5f * scaleY , -0.5f * scaleZ},uv[2],norm };
	vertices[11] = { { -0.5f * scaleX , sign *  0.5f * scaleY ,  0.5f * scaleZ},uv[3],norm };
	//y_back
	norm = -1.0f * outer * UnitVec3Y;
	vertices[12] = { { sign *  0.5f * scaleX ,-0.5f * scaleY , -0.5f * scaleZ},uv[0],norm };
	vertices[13] = { { sign *  0.5f * scaleX ,-0.5f * scaleY ,  0.5f * scaleZ},uv[1],norm };
	vertices[14] = { { sign * -0.5f * scaleX ,-0.5f * scaleY , -0.5f * scaleZ},uv[2],norm };
	vertices[15] = { { sign * -0.5f * scaleX ,-0.5f * scaleY ,  0.5f * scaleZ},uv[3],norm };
	//z_front
	norm = outer * UnitVec3Z;
	vertices[16] = { {  0.5f * scaleX , sign *  0.5f * scaleY , 0.5f * scaleZ},uv[0],norm };
	vertices[17] = { { -0.5f * scaleX , sign *  0.5f * scaleY , 0.5f * scaleZ},uv[1],norm };
	vertices[18] = { {  0.5f * scaleX , sign * -0.5f * scaleY , 0.5f * scaleZ},uv[2],norm };
	vertices[19] = { { -0.5f * scaleX , sign * -0.5f * scaleY , 0.5f * scaleZ},uv[3],norm };
	//z_back
	norm = -1.0f * outer * UnitVec3Z;
	vertices[20] = { { -0.5f * scaleX , sign *  0.5f * scaleY , -0.5f * scaleZ},uv[0],norm };
	vertices[21] = { {  0.5f * scaleX , sign *  0.5f * scaleY , -0.5f * scaleZ},uv[1],norm };
	vertices[22] = { { -0.5f * scaleX , sign * -0.5f * scaleY , -0.5f * scaleZ},uv[2],norm };
	vertices[23] = { {  0.5f * scaleX , sign * -0.5f * scaleY , -0.5f * scaleZ},uv[3],norm };
	m_bufferIndex.vertexIndices[0] = m_renderer->AllocateVertexBuffer(
		(void*)vertices, 24 * sizeof(VertexNormal), sizeof(VertexNormal));

	//インデックスバッファ
	unsigned short indices[36]{};
	for (int i = 0;i < 6;i++)
	{
		unsigned short index = 4 * i;
		unsigned short s = 1;
		for (int j = 0;j < 6;j++)
		{
			indices[6 * i + j] = index;
			index += s;

			if (j == 2)
			{
				index = 4 * i + 3;
				s = -1;
			}
		}
	}
	m_bufferIndex.indexIndices[0] = m_renderer->AllocateIndexBuffer(
		(void*)indices, 36 * sizeof(unsigned short));

	//定数バッファ
	ModelData mData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&mData, sizeof(ModelData));

	//シェーダリソース
	for (size_t i = 0;i < texturePath.size();i++)
	{
		m_bufferIndex.imgData[i] = m_renderer->AllocateShaderResource(
			texturePath[i], ddsFlag);
	}

	//どれか１つでも不備があった場合はバッファを削除
	if (m_bufferIndex.vertexIndices[0] == -1 ||
		m_bufferIndex.indexIndices[0]  == -1 ||
		m_bufferIndex.constIndices[0]  == -1)
	{
		ReleaseBuffer();
		return;
	}
	for (size_t i = 0;i < texturePath.size();i++)
	{
		if (m_bufferIndex.imgData[i].imgIndex == -1)
		{
			ReleaseBuffer();
			return;
		}
	}

	m_enabled = true;
}

BoxMesh::~BoxMesh()
{
}

void BoxMesh::Draw()
{
	if (!m_enabled) return;

	ModelData mData(m_owner->GetWorldMatrix(), m_curMaterial);
	m_renderer->UploadConstBuffer(m_bufferIndex.constIndices[0],
		(void*)&mData, sizeof(ModelData));

	m_renderer->SetShader(m_shaderClassIndex);
	m_renderer->SetVertexBufferViewToCommandList(m_bufferIndex.vertexIndices[0]);
	m_renderer->SetIndexBufferViewToCommandList(m_bufferIndex.indexIndices[0]);
	m_renderer->SetConstBufferViewToCommandList(1, m_bufferIndex.constIndices[0]);
	if (m_bufferIndex.imgData.size() == 1)
	{
		m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[0].imgIndex);
		m_renderer->DrawMeshIndexed(36, 0);
	}
	else
	{
		for (size_t i = 0;i < m_bufferIndex.imgData.size();i++)
		{
			m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[i].imgIndex);
			m_renderer->DrawMeshIndexed(6, (int)(6 * i));
		}
	}
}