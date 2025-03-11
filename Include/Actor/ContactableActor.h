#pragma once

#include "BoxActor.h"

/// <summary>
/// ÚG‰Â”\‚È‹@”\‚ğ‚Â
/// </summary>
class ContactableActor : public BoxActor
{
public:
	ContactableActor(class Game* game, int shaderClassIndex,
		std::vector<const wchar_t*>& texturePath,
		const XMFLOAT3& pos = ZeroVec3, const XMFLOAT3& vel = ZeroVec3,
		const XMFLOAT3& angle = ZeroVec3, const XMFLOAT3& aVel = ZeroVec3,
		float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
		bool ddsFlag = false);
	~ContactableActor();

	void UpdateActor(float deltaTime) override;
	void Draw() override;
};