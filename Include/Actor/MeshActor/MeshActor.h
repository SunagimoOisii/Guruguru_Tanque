#pragma once

#include "Actor3D.h"
#include "Mesh.h"
#include <Memory>

/// <summary>
/// メッシュを扱うベースとなるクラス<para></para>
/// Meshクラス変数を持つが継承先でテンプレート指定を行い、どのようなMeshか具体化して生成させる
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