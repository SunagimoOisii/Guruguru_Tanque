#include "SphereMesh.h"
#include "Actor3D.h"
#include "Renderer.h"

SphereMesh::SphereMesh(Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* texturePath,
	bool outer, float rad, UINT divisionV, UINT divisionH,
	bool oneSide, float scaleU, float scaleV,
	bool ddsFlag)
	:Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 1)
	, m_indicesNum(0)
{
	//球体として成立するに必須な垂直,水平の分割数を確保
	if (divisionV < 2) divisionV = 2;
	if (divisionH < 3) divisionH = 3;

	//分割された角度とUV座標のステップ値を計算
	float angleV = XM_PI / divisionV;
	float sign	 = (outer) ? -1.0f : 1.0f;
	float angleH = sign * XM_2PI / divisionH;
	float stepU  = scaleU / divisionH;
	float stepV  = scaleV / divisionV;

//各種バッファの生成
	//頂点バッファ
	int vNum = (divisionV - 1) * (divisionH + 1) + 2;
	std::vector<VertexNormal> vertices(vNum);
	int pos = 0;
	for (UINT i = 0;i <= divisionV;i++)
	{
		if (i == 0)
		{
			vertices[pos] = { {0.0f,0.0f,rad},{0.0f,0.0f},-1.0f * sign * UnitVec3Z };
			pos++;
		}
		else if (i < divisionV)
		{
			float rh = rad * XMScalarSin(i * angleV);
			float z  = rad * XMScalarCos(i * angleV);
			for (UINT j = 0;j <= divisionH;j++)
			{
				XMFLOAT3 p = { rh * XMScalarCos(j * angleH),rh * XMScalarSin(j * angleH),z };
				XMFLOAT3 normal = -1.0f * sign * Normalize(p);
				vertices[pos] = { p,{stepU * j,stepV * i},normal };
				pos++;
			}
		}
		else
		{
			if (divisionV == 2 && oneSide)
			{
				vertices[pos] = { ZeroVec3,{0.0f,scaleV},sign * UnitVec3Z };
				pos++;
			}
			else
			{
				vertices[pos] = { {0.0f,0.0f,-rad},{0.0f,scaleV},sign * UnitVec3Z };
				pos++;
			}
		}
	}
	m_bufferIndex.vertexIndices[0] = m_renderer->AllocateVertexBuffer(
		(void*)vertices.data(), vNum * sizeof(VertexNormal), sizeof(VertexNormal));

	//インデックスバッファ
	int iNum = 6 * divisionH * (divisionV - 1);
	pos = 0;
	unsigned short sIndex = 0;
	unsigned short eIndex = vNum - 1;
	std::vector<unsigned short> indices(iNum);
	for (unsigned short i = 1;i <= divisionH;i++)
	{
		indices[pos++] = i;
		indices[pos++] = sIndex;
		indices[pos++] = i + 1;
	}
	for (unsigned short i = 1;i <= (divisionV - 2);i++)
	{
		unsigned short baseV = 1 + (i - 1) * (divisionH + 1);
		for (unsigned short j = 1;j <= divisionH;j++)
		{
			unsigned short baseH = baseV + j - 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + divisionH + 2;
			indices[pos++] = baseH + divisionH + 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + 1;
			indices[pos++] = baseH + divisionH + 2;
		}
	}
	unsigned short base = 1 + (divisionV - 2) * (divisionH + 1);
	for (unsigned short i = 1;i <= divisionH;i++)
	{
		indices[pos++] = base + i - 1;
		indices[pos++] = base + i;
		indices[pos++] = eIndex;
	}
	m_bufferIndex.indexIndices[0] = m_renderer->AllocateIndexBuffer(
		(void*)indices.data(), iNum * sizeof(unsigned short));

	//定数バッファ
	ModelData mData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&mData, sizeof(ModelData));

	//シェーダリソース
	m_bufferIndex.imgData[0] = m_renderer->AllocateShaderResource(
		texturePath, ddsFlag);

	//どれか１つでも不備があった場合はバッファを削除
	if (m_bufferIndex.vertexIndices[0] == -1 ||
		m_bufferIndex.indexIndices[0]  == -1 ||
		m_bufferIndex.constIndices[0]  == -1 ||
		m_bufferIndex.imgData[0].imgIndex == -1)
	{
		ReleaseBuffer();
		return;
	}

	m_indicesNum = iNum;
	m_enabled	 = true;
}

SphereMesh::~SphereMesh()
{
}

void SphereMesh::Draw()
{
	if (!m_enabled) return;

	ModelData mData(m_owner->GetWorldMatrix(), m_curMaterial);
	m_renderer->UploadConstBuffer(m_bufferIndex.constIndices[0],
		(void*)&mData, sizeof(ModelData));

	m_renderer->SetShader(m_shaderClassIndex);
	m_renderer->SetVertexBufferViewToCommandList(m_bufferIndex.vertexIndices[0]);
	m_renderer->SetIndexBufferViewToCommandList(m_bufferIndex.indexIndices[0]);
	m_renderer->SetConstBufferViewToCommandList(1, m_bufferIndex.constIndices[0]);
	m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[0].imgIndex);
	m_renderer->DrawMeshIndexed(m_indicesNum, 0);
}