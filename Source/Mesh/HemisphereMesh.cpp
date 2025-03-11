#include "HemisphereMesh.h"
#include "Actor3D.h"
#include "Renderer.h"

HemisphereMesh::HemisphereMesh(class Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* texturePath,
	bool outer, float rad, UINT divisionV, UINT divisionH,
	float circleX, float circleY, float imgRad, float angleScale,
	bool orthographic, bool ddsFlag)
	:Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 1)
	, m_indicesNum(0)
{
//各種バッファの生成
	//シェーダリソース
	//テクスチャの幅と高さを取得するために先にこれを生成
	m_bufferIndex.imgData[0] = m_renderer->AllocateShaderResource(
		texturePath, ddsFlag);

	if (divisionV < 1) divisionV = 1;
	if (divisionH < 3) divisionH = 3;
	float imgW	  = (float)m_bufferIndex.imgData[0].width;
	float imgH	  = (float)m_bufferIndex.imgData[0].height;
	float circleU = circleX / imgW;
	float circleV = circleY / imgH;
	float radU	  = imgRad / imgW;
	float radV    = imgRad / imgH;
	float angleV  = XM_PIDIV2 / divisionV;
	float sign    = (outer) ? -1.0f : 1.0f;
	float angleH  = sign * XM_2PI / divisionH;

	//頂点バッファ
	int vNum = divisionV * (divisionH + 1) + 1;
	std::vector<VertexNormal> vertices(vNum);
	int pos = 0;
	for (UINT i = 0;i <= divisionV;i++)
	{
		if (i == 0)
		{
			vertices[pos] = { {0.0f,0.0f,rad},{circleU,circleV},-1.0f * sign * UnitVec3Z };
			pos++;
		}
		else
		{
			float rh = rad * XMScalarSin(i * angleV);
			float z  = rad * XMScalarCos(i * angleV);
			for (UINT j = 0;j <= divisionH;j++)
			{
				XMFLOAT3 p = { rh * XMScalarCos(j * angleH),rh * XMScalarSin(j * angleH),z };

				float phi, proj;
				if (orthographic)
				{
					phi  = i * angleV * angleScale;
					proj = XMScalarSin(phi);
				}
				else
				{
					phi  = 0.5f * i * angleV * angleScale;
					proj = XMScalarSin(phi) / XMScalarCos(phi);
				}
				XMFLOAT2 uv = { radU * proj * XMScalarCos(-(j * angleH)) + circleU,
					radV * proj * XMScalarSin(-(j * angleH)) + circleV };
				XMFLOAT3 normal = -1.0f * sign * Normalize(p);
				vertices[pos] = { p,uv,normal };
				pos++;
			}
		}
	}
	m_bufferIndex.vertexIndices[0] = m_renderer->AllocateVertexBuffer(
		(void*)vertices.data(), vNum * sizeof(VertexNormal), sizeof(VertexNormal));

	//インデックスバッファ
	int iNum = 3 * divisionH * (2 * divisionV - 1);
	pos = 0;
	std::vector<unsigned short> indices(iNum);
	for (unsigned short i = 1;i <= divisionH;i++)
	{
		indices[pos++] = i;
		indices[pos++] = 0;
		indices[pos++] = i + 1;
	}
	for (unsigned short i = 1;i <= (divisionV - 1);i++)
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
	m_bufferIndex.indexIndices[0] = m_renderer->AllocateIndexBuffer(
		(void*)indices.data(), iNum * sizeof(unsigned short));

	//定数バッファ
	ModelData mData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&mData, sizeof(ModelData));

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
	m_enabled    = true;
}

HemisphereMesh::~HemisphereMesh()
{

}

void HemisphereMesh::Draw()
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