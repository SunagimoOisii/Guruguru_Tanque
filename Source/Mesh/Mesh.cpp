#include "Mesh.h"
#include "Actor3D.h"
#include "Renderer.h"

Mesh::Mesh(Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, int vertexBufferNum, int indexBufferNum,
	int constBufferNum, int imageBufferNum)
	: m_owner(owner)
	, m_renderer(renderer)
	, m_shaderClassIndex(shaderClassIndex)
	, m_bufferIndex(vertexBufferNum, indexBufferNum, constBufferNum, imageBufferNum)
	, m_enabled(false)
{
}

Mesh::~Mesh()
{
	ReleaseBuffer();
}

void Mesh::SetOriginalMaterial(MeshMaterial material)
{
	m_oriMaterial = material;
	m_curMaterial = material;
}

void Mesh::SetCurrentMaterial(MeshMaterial material)
{
	m_curMaterial = material;
}

void Mesh::ResetMaterial()
{
	m_curMaterial = m_oriMaterial;
}

void Mesh::ReleaseBuffer()
{
	for (size_t i = 0;i < m_bufferIndex.vertexIndices.size();i++)
	{
		if (m_bufferIndex.vertexIndices[i] <= 0) continue;
		m_renderer->ReleaseVertexBuffer(m_bufferIndex.vertexIndices[i]);
	}
	for (size_t i = 0;i < m_bufferIndex.indexIndices.size();i++)
	{
		if (m_bufferIndex.indexIndices[i] <= 0) continue;
		m_renderer->ReleaseIndexBuffer(m_bufferIndex.indexIndices[i]);
	}
	for (size_t i = 0;i < m_bufferIndex.constIndices.size();i++)
	{
		if (m_bufferIndex.constIndices[i] <= 0) continue;
		m_renderer->ReleaseConstBuffer(m_bufferIndex.constIndices[i]);
	}
	for (size_t i = 0;i < m_bufferIndex.imgData.size();i++)
	{
		if (m_bufferIndex.imgData[i].imgIndex <= 0) continue;
		m_renderer->ReleaseShaderResource(m_bufferIndex.imgData[i]);
	}

	m_bufferIndex.vertexIndices.clear();
	m_bufferIndex.indexIndices.clear();
	m_bufferIndex.constIndices.clear();
	m_bufferIndex.imgData.clear();
}