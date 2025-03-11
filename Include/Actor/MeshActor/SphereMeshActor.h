#pragma once

#include "MeshActor.h"

class SphereMeshActor : public MeshActor
{
public:
	SphereMeshActor(class Game* game,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float radius, UINT divisionV, UINT divisionH,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		bool oneSide = false, float scaleU = 1.0f, float scaleV = 1.0f,
		bool ddsFlag = false);
	virtual ~SphereActor();

	bool IsEnabled() const;
	Shape::Sphere GetBoundingSphere() const { return Shape::Sphere(m_pos, m_radius); }

	virtual void UpdateActor(float deltaTime) override;

	virtual void Draw() override;

protected:
	float m_radius;
};