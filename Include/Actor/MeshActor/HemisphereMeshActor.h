#pragma once

#include "MeshActor.h"

class HemisphereMeshActor : public MeshActor
{
public:
	HemisphereMeshActor(class Game* game,
		int shaderClassIndex, const wchar_t* texturePath,
		bool outer, float rad, UINT divisionV, UINT divisionH,
		float circleX, float circleY, float imgRad,
		float angleScale = 1.0f, bool orthographic = false,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		bool ddsFlag = false);
	virtual ~HemisphereActor();

	bool IsEnabled() const;

	virtual void UpdateActor(float deltaTime) override;
	virtual void Draw() override;
};