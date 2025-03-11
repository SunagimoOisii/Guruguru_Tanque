#pragma once

#include "Mesh.h"

/// <summary>
/// �������b�V���N���X
/// </summary>
class HemisphereMesh : public Mesh
{
public:
	/// <summary>
	/// �e�N�X�`�� : uv���W��0�`1����Ƃ��AScaleU,V�Ńe�N�X�`���̌J��Ԃ��񐔂��w��<para></para>
	/// uv���W�͋��ᓊ�e�ɂ���ă}�b�s���O����
	/// </summary>
	/// <param name="outer">���b�V���̓��O�ǂ��炩�猩�ĕ\���Ƃ��邩�̐ݒ�</param>
	/// <param name="divisionU,V">���̐���,�����̕�����</param>
	/// <param name="orthographic">���ˉe�����̎ˉe��</param>
	HemisphereMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float rad, UINT divisionV, UINT divisionH,
		float circleX, float circleY, float imgRad, float angleScale,
		bool orthographic, bool ddsFlag = false);
	~HemisphereMesh();

	void Draw() override;

private:
	int m_indicesNum;
};