#pragma once

#include "Mesh.h"
#include <vector>

/// <summary>
/// ������,�����̃��b�V���N���X
/// </summary>
class BoxMesh : public Mesh
{
public:
	/// <summary>
	/// �X�P�[�� : ���_�𒆐S��1m*1m*1m��Ŕ{��ScaleX,Y,Z���w��<para></para>
	/// �e�N�X�`�� : 1or6���ݒ�\
	/// </summary>
	/// <param name="outer">���b�V���̓��O�ǂ��炩�猩�ĕ\���Ƃ��邩�̐ݒ�</param>
	BoxMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, std::vector<const wchar_t*>& texturePath,
		bool outer, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		bool ddsFlag = false);
	~BoxMesh();

	void Draw() override;
};