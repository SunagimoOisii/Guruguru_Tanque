#pragma once

#include "BoxActor.h"

class WallActor : public BoxActor
{
public:
	WallActor(class Game* game, int shaderClassIndex, std::vector<const wchar_t*>& texturePath, 
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		bool ddsFlag = false);
	~WallActor();

	Shape::RotateRect GetBoundingRotatedRect() const;

	void UpdateActor(float deltaTime) override;
	void Draw() override;

private:
	XMFLOAT2 m_scaleXY;
	float m_height;
};