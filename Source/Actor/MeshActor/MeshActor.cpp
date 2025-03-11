#include "MeshActor.h"
#include "Game.h"

MeshActor::MeshActor(Game* game,
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& avel)
	:Actor3D(game, pos, vel, angle, avel)
{
}

MeshActor::~MeshActor()
{
}

void MeshActor::SetOriginalMaterial(MeshMaterial material)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->SetOriginalMaterial(material);
}

void MeshActor::SetCurrentMaterial(MeshMaterial material)
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->SetCurrentMaterial(material);
}

void MeshActor::ResetMaterial()
{
	if (m_mesh.get() == nullptr) return;

	m_mesh->ResetMaterial();
}