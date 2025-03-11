#include "WallActor.h"
#include "Game.h"
#include "Collider.h"

WallActor::WallActor(class Game* game, int shaderClassIndex, std::vector<const wchar_t*>& texturePath, 
	const XMFLOAT3& pos, const XMFLOAT3& vel,
	const XMFLOAT3& angle, const XMFLOAT3& aVel,
	float scaleX, float scaleY, float scaleZ,
	bool ddsFlag)
	: BoxActor(game, shaderClassIndex, texturePath, true,
		pos, vel, angle, aVel,
		scaleX, scaleY, scaleZ, ddsFlag)
	, m_scaleXY(scaleX,scaleY)
	, m_height(scaleZ)
{
	SetOriginalMaterial(MeshMaterial(XMFLOAT3(0.95f, 0.5f, 0.0f),
		XMFLOAT3(0.6f, 0.3f, 0.0f), 8.0f, XMFLOAT3(0.2f, 0.1f, 0.0f)));
}

WallActor::~WallActor()
{
}

Shape::RotateRect WallActor::GetBoundingRotatedRect() const
{
	return RotateRect({m_pos.x,m_pos.y}, m_scaleXY.x, m_scaleXY.y, m_angle.z);
}

void WallActor::UpdateActor(float deltaTime)
{
}

void WallActor::Draw()
{
	if (!m_visible) return;

	BoxActor::Draw();
}