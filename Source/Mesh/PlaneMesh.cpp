#include "PlaneMesh.h"
#include "Actor3D.h"
#include "Renderer.h"

PlaneMesh::PlaneMesh(class Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* texturePath,
	float scaleX, float scaleY, float scaleU, float scaleV, bool ddsFlag)
	:Mesh(owner, renderer, shaderClassIndex, 1, 0, 1, 1) //トポロジーをstripにするのでインデックスバッファ不要
{
//各種データのバッファ生成
	//頂点バッファ
	VertexNormal vertices[4];
	vertices[0] = { {-0.5f * scaleX,-0.5f * scaleY,0.0f},{0.0f,scaleV},UnitVec3Z };
	vertices[1] = { {0.5f * scaleX,-0.5f * scaleY,0.0f},{0.0f,0.0f},UnitVec3Z };
	vertices[2] = { {-0.5f * scaleX,0.5f * scaleY,0.0f},{scaleU,scaleV},UnitVec3Z };
	vertices[3] = { {0.5f * scaleX,0.5f * scaleY,0.0f},{scaleU,0.0f},UnitVec3Z };
	m_bufferIndex.vertexIndices[0] = m_renderer->AllocateVertexBuffer(
		(void*)vertices, 4 * sizeof(VertexNormal), sizeof(VertexNormal));

	//定数バッファ
	ModelData mData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&mData, sizeof(ModelData));

	//シェーダリソース
	m_bufferIndex.imgData[0] = m_renderer->AllocateShaderResource(
		texturePath, ddsFlag);

	//どれか１つでも不備があった場合はバッファを削除
	if (m_bufferIndex.vertexIndices[0] == -1 ||
		m_bufferIndex.constIndices[0] == -1  ||
		m_bufferIndex.imgData[0].imgIndex == -1)
	{
		ReleaseBuffer();
		return;
	}

	m_enabled = true;
}

PlaneMesh::~PlaneMesh()
{
}

void PlaneMesh::Draw()
{
	if (!m_enabled) return;

	ModelData mData(m_owner->GetWorldMatrix(), m_curMaterial);
	m_renderer->UploadConstBuffer(m_bufferIndex.constIndices[0],
		(void*)&mData, sizeof(ModelData));

	m_renderer->SetShader(m_shaderClassIndex);
	m_renderer->SetVertexBufferViewToCommandList(m_bufferIndex.vertexIndices[0]);
	m_renderer->SetConstBufferViewToCommandList(1, m_bufferIndex.constIndices[0]);
	m_renderer->SetShaderResourceViewToCommandList(0, m_bufferIndex.imgData[0].imgIndex);
	m_renderer->DrawMeshStrip(4);
}