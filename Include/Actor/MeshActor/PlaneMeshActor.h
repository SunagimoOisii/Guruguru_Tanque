#pragma once

#include "MeshActor.h"

class PlaneMeshActor : public MeshActor
{
public:
	PlaneMeshActor(class Game* game,
		int shaderClassIndex, const wchar_t* texturePath,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	virtual ~PlaneActor();

	bool IsEnabled() const;
	Shape::InfinitePlane GetBoundingInfinitePlane()
	{
		return Shape::InfinitePlane(GetUp(), m_pos); 
	}
	Shape::FinitePlane GetBoundingFinitePlane()
	{ 
		return Shape::FinitePlane(m_pos, GetForward(), GetRight(), m_scaleX, m_scaleY);;
	}

	virtual void UpdateActor(float deltaTime) override;

	virtual void Draw() override;

protected:
	float m_scaleX;
	float m_scaleY;
};