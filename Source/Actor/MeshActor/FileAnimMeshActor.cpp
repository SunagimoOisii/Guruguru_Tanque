#include "FileAnimMeshActor.h"
#include "FileAnimMesh.h"
#include "Game.h"
#include "Renderer.h"

FileAnimMeshActor::FileAnimMeshActor(Game* game, int shaderClassIndex, const wchar_t* filePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	const wchar_t* imgFilePrefix, bool ddsFlag)
	: MeshActor(game, pos, vel, angle, aVel)
	, m_elapsedAnimTime(0.0f)
{
	m_mesh = std::make_unique<FileAnimMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, filePath, imgFilePrefix, ddsFlag);
}

FileAnimMeshActor::~FileAnimMeshActor()
{
}

bool FileAnimMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}

void FileAnimMeshActor::UpdateActor(float deltaTime)
{
	FileAnimMesh* mesh = (FileAnimMesh*)m_mesh.get();

	//アニメーションデータの経過時間の管理と
	//FileAnimMeshのインデックス切り替えによりアニメーションを再生
	m_elapsedAnimTime += deltaTime;
	if (m_elapsedAnimTime >= mesh->GetInterval())
	{
		m_elapsedAnimTime = 0.0f;
		UINT index = (mesh->GetAnimIndex() + 1) % mesh->GetFrameNum();
		mesh->SetAnimIndex(index);
	}

	CalcWorldTransform();
}

void FileAnimMeshActor::Draw()
{
	m_mesh->Draw();
}