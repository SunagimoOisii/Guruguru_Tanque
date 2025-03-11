#pragma once

#include "Mesh.h"

/// <summary>
/// ���ʂ̋�`���b�V���N���X
/// </summary>
class PlaneMesh : public Mesh
{
public:
	/// <summary>
	/// �X�P�[�� : XY���ʏ�ɔz�u���T�C�Y��1m*1m��Ŕ{��ScaleX,Y,Z���w��<para></para>
	/// �e�N�X�`�� : uv���W��0�`1����Ƃ��AScaleU,V�Ńe�N�X�`���̌J��Ԃ��񐔂��w��
	/// </summary>
	PlaneMesh(class Actor3D* owner, const std::unique_ptr<class Renderer>& renderer,
		int shaderClassIndex, const wchar_t* texturePath,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	~PlaneMesh();

	void Draw() override;
};