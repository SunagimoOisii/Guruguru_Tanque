#include "GridMesh.h"
#include "Actor3D.h"
#include "Renderer.h"

GridMesh::GridMesh(Actor3D* owner, const std::unique_ptr<Renderer>& renderer,
	int shaderClassIndex, const wchar_t* texturePath,
	const GridHeightMap& map, float scaleU, float scaleV,
	bool ddsFlag)
	: Mesh(owner, renderer, shaderClassIndex, 1, 1, 1, 1)
{
	size_t vNum = (size_t)(map.xVertNum * map.yVertNum);
	float uStep = scaleU / (float)(map.xVertNum - 1);
	float vStep = scaleV / (float)(map.yVertNum - 1);

	//���_�̐���
	std::vector<VertexNormal> vertices(vNum);
	for (int y = 0;y < map.yVertNum;y++)
	{
		for (int x = 0;x < map.xVertNum;x++)
		{
			int index = x + y * map.xVertNum;
			vertices[index] = { map.heightMap[index],
				{uStep * x,vStep * y},UnitVec3Z };
		}
	}

	//�@���̌v�Z
	//1�̊i�q�ɂ�2�̎O�p�`�|���S�������蓖�ĂĂ��邽�߁A
	//���_�͂��̈ʒu�ɂ���đ�����|���S���̐����قȂ�
	//2�ȏ�ɑ����ꍇ�̓|���S�����ɖ@�����v�Z���A
	//���̕��ς𒸓_�̖@���Ƃ��Đݒ肷��
	for (int y = 0;y < map.yVertNum;y++)
	{
		for (int x = 0;x < map.xVertNum;x++)
		{
			int index = x + y * map.xVertNum;

			//�����|���S����:1��
			if (x == map.xVertNum - 1 && y == 0)
			{
				vertices[index].normal = CalcNormalVector(
					vertices[index + map.xVertNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
			}
			else if (x == 0 && y == map.yVertNum - 1)
			{
				vertices[index].normal = CalcNormalVector(
					vertices[index - map.xVertNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
			}
			//�����|���S����:2��
			else if (x == 0 && y == 0)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2);
			}
			else if (x == map.xVertNum - 1 && y == map.yVertNum - 1)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2);
			}
			//�����|���S����:3��
			else if (y == 0)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index + map.xVertNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum].pos - vertices[index].pos);
				XMFLOAT3 normal3 = CalcNormalVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2 + normal3);
			}
			else if (y == map.yVertNum - 1)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum].pos - vertices[index].pos);
				XMFLOAT3 normal3 = CalcNormalVector(
					vertices[index - map.xVertNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2 + normal3);
			}
			else if (x == 0)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index - map.xVertNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos);
				XMFLOAT3 normal3 = CalcNormalVector(
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2 + normal3);
			}
			else if (x == map.xVertNum - 1)
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos);
				XMFLOAT3 normal3 = CalcNormalVector(
					vertices[index + map.xVertNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2 + normal3);
			}
			else
			{
				XMFLOAT3 normal1 = CalcNormalVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos);
				XMFLOAT3 normal2 = CalcNormalVector(
					vertices[index - map.xVertNum - 1].pos - vertices[index].pos,
					vertices[index - map.xVertNum].pos - vertices[index].pos);
				XMFLOAT3 normal3 = CalcNormalVector(
					vertices[index - map.xVertNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
				XMFLOAT3 normal4 = CalcNormalVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos);
				XMFLOAT3 normal5 = CalcNormalVector(
					vertices[index + map.xVertNum + 1].pos - vertices[index].pos,
					vertices[index + map.xVertNum].pos - vertices[index].pos);
				XMFLOAT3 normal6 = CalcNormalVector(
					vertices[index + map.xVertNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);

				vertices[index].normal = Normalize(normal1 + normal2 + normal3 +
												   normal4 + normal5 + normal6);
			}
		}
	}

//�e��o�b�t�@�̐���
	//���_�o�b�t�@
	m_bufferIndex.vertexIndices[0] = m_renderer->AllocateVertexBuffer(
		(void*)vertices.data(), vertices.size() * sizeof(VertexNormal),
		sizeof(VertexNormal));

	//�C���f�b�N�X�o�b�t�@
	m_indicesNum = 6 * (map.xVertNum - 1) * (map.yVertNum - 1);
	std::vector<unsigned short> indices(m_indicesNum);
	for (int yPoly = 0;yPoly < map.yVertNum - 1;yPoly++)
	{
		for (int xPoly = 0;xPoly < map.xVertNum - 1;xPoly++)
		{
			int v = xPoly + yPoly * map.xVertNum;
			int index = 6 * (xPoly + yPoly * (map.xVertNum - 1));
			indices[index]	   = v;
			indices[index + 1] = v + map.xVertNum + 1;
			indices[index + 2] = v + map.xVertNum;
			indices[index + 3] = v;
			indices[index + 4] = v + 1;
			indices[index + 5] = v + map.xVertNum + 1;
		}
	}
	m_bufferIndex.indexIndices[0] = m_renderer->AllocateIndexBuffer(
		(void*)indices.data(), m_indicesNum * sizeof(unsigned short));

	//�萔�o�b�t�@
	ModelData mData;
	m_bufferIndex.constIndices[0] = m_renderer->AllocateConstBuffer(
		(void*)&mData, sizeof(ModelData));

	//�V�F�[�_���\�[�X
	m_bufferIndex.imgData[0] = m_renderer->AllocateShaderResource(
		texturePath, ddsFlag);

	//�ǂꂩ�P�ł��s�����������ꍇ�̓o�b�t�@���폜
	if (m_bufferIndex.vertexIndices[0]    == -1 ||
		m_bufferIndex.indexIndices[0]     == -1 ||
		m_bufferIndex.constIndices[0]     == -1 ||
		m_bufferIndex.imgData[0].imgIndex == -1)
	{
		ReleaseBuffer();
		return;
	}

	m_enabled = true;
}

GridMesh::~GridMesh()
{
}

void GridMesh::Draw()
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