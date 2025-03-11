#pragma once

#include "Actor3D.h"
#include "Mesh.h"
#include <Memory>

/// <summary>
/// ���b�V���������x�[�X�ƂȂ�N���X<para></para>
/// Mesh�N���X�ϐ��������p����Ńe���v���[�g�w����s���A�ǂ̂悤��Mesh����̉����Đ���������
/// </summary>
class MeshActor : public Actor3D
{
public:
	MeshActor(class Game* game,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& avel = ZeroVec3);
	virtual ~MeshActor();

	void SetOriginalMaterial(MeshMaterial material);
	void SetCurrentMaterial(MeshMaterial material);
	void ResetMaterial();

protected:
	std::unique_ptr<Mesh> m_mesh;
};