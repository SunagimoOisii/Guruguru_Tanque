#pragma once

#include "Mesh.h"

/// <summary>
/// �����b�V���N���X
/// </summary>
class SphereMesh : public Mesh
{
public:
	/// <summary>
	/// �e�N�X�`�� : uv���W��0�`1����Ƃ��AScaleU,V�Ńe�N�X�`���̌J��Ԃ��񐔂��w��<para></para>
	/// uv���W�͋����W���e�ɂ���ă}�b�s���O����
	/// </summary>
	/// <param name="outer">���b�V���̓��O�ǂ��炩�猩�ĕ\���Ƃ��邩�̐ݒ�</param>
	/// <param name="divisionU,V">���̐���,�����̕�����</param>
	SphereMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float rad, UINT divisionV, UINT divisionH,
		bool oneSide = false, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~SphereMesh();

	void Draw() override;

private:
	int m_indicesNum;
};