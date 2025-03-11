#include "FileMeshActor.h"
#include "FileMesh.h"
#include "Game.h"
#include "Renderer.h"

FileMeshActor::FileMeshActor(Game* game, int shaderClassIndex, const wchar_t* filePath,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	const wchar_t* imgFilePrefix, bool ddsFlag)
	:MeshActor(game, pos, vel, angle, aVel)
{
	m_mesh = std::make_unique<FileMesh>(this, m_game->GetRenderer(),
		shaderClassIndex, filePath, imgFilePrefix, ddsFlag);
}

FileMeshActor::~FileMeshActor()
{
}

void FileMeshActor::UpdateActor(float deltaTime)
{
	CalcWorldTransform();
}

void FileMeshActor::Draw()
{
	m_mesh->Draw();
}

bool FileMeshActor::IsEnabled() const
{
	return m_mesh->IsEnabled();
}