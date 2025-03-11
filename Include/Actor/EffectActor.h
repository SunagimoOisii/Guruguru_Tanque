#pragma once

#include "BlockAnimActor.h"

class EffectActor : public BlockAnimActor
{
public:
	EffectActor(class Game* game, const std::wstring& imgPath, bool visible,
		const std::vector<std::vector<UINT>>& indices,
		UINT lane, float interval, bool ddsFlag = false,
		UINT wNum = 3, UINT hNum = 4, float sx = 1.0f, float sy = 1.0f,
		const XMFLOAT2& pos = ZeroVec2, const XMFLOAT2& vel = ZeroVec2,
		INT loopNum = BlockAnimActor::LoopInfinity, float delayTime = 0.0f);
	virtual ~EffectActor();

	virtual void UpdateActor(float deltaTime) override;

private:
	float m_delayTime;
};

void CreateExplosion(class Game* game, const std::wstring& imgPath,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, bool ddsFlag,
	UINT wNum, UINT hNum, float sx, float sy, const XMFLOAT2& pos,
	UINT eNum, float eInterval, float eWidth, float eHeight, float eVel);