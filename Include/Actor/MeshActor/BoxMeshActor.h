#pragma once

#include "MeshActor.h"

class BoxMeshActor : public MeshActor
{
public:
	BoxMeshActor(class Game* game, int shaderClassIndex,
		std::vector<const wchar_t*>& texturePath, bool outer,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		bool ddsFlag = false);
	virtual ~BoxActor();

	bool IsEnabled() const;
	OBB GetBoundingBox()
	{ 
		return OBB(m_pos, GetForward(), GetRight(), GetUp(), m_scale); 
	}

	virtual void UpdateActor(float deltaTime) override;
	virtual void Draw() override;

protected:
	XMFLOAT3 m_scale;
};