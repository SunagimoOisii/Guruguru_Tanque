#pragma once

#include <memory>

#include "GameUtil.h"

/// <summary>
/// ���b�V�����������߂̊��N���X<para></para>
/// ���_�o�b�t�@,�C���f�b�N�X�o�b�t�@,�萔�o�b�t�@,�V�F�[�_���\�[�X��ێ�<para></para>
/// �Ǘ��p�̍\���̂Ƃ���BufferIndex��݌v
/// </summary>
class Mesh
{
public:
	Mesh(class Actor3D* owner,const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, int vertexBufferNum, int indexBufferNum,
		int constBufferNum, int imageBufferNum);
	virtual ~Mesh();

	virtual void Draw() = 0;

	bool IsEnabled() const { return m_enabled; }
	void SetOriginalMaterial(MeshMaterial material);
	void SetCurrentMaterial(MeshMaterial material);
	void ResetMaterial();

protected:
	void ReleaseBuffer();

	class Actor3D*  m_owner;
	const std::unique_ptr<class Renderer>& m_renderer;
	int			    m_shaderClassIndex;
	BufferIndex		m_bufferIndex;

	MeshMaterial m_oriMaterial;
	MeshMaterial m_curMaterial;

	bool m_enabled;
};