#include "EffectActor.h"
#include "Game.h"

EffectActor::EffectActor(Game* game, const std::wstring& imgPath, bool visible,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, bool ddsFlag,
	UINT wNum, UINT hNum, float sx, float sy,
	const XMFLOAT2& pos, const XMFLOAT2& vel,INT loopNum,float delayTime)
	: BlockAnimActor(game, imgPath, visible, indices, lane, interval, ddsFlag,
		wNum, hNum, sx, sy, pos, vel,BlockAnimActor::AnimState::Play,loopNum)
	,m_delayTime(delayTime)
{
	if (delayTime > 0.0f)
	{
		m_visible = false;
	}
}

EffectActor::~EffectActor()
{
}

void EffectActor::UpdateActor(float deltaTime)
{
	if (m_delayTime > 0.0f)
	{
		m_delayTime -= deltaTime;
		if (m_delayTime > 0.0f)
		{
			return;
		}
		else
		{
			m_visible = true;
		}
	}
	m_pos += m_vel * deltaTime;
	BlockAnimActor::UpdateAnim(deltaTime);
	if (GetAnimState() == AnimState::Stop)
	{
		m_dead = true;
	}
}


void CreateExplosion(class Game* game, const std::wstring& imgPath,
	const std::vector<std::vector<UINT>>& indices,
	UINT lane, float interval, bool ddsFlag,
	UINT wNum, UINT hNum, float sx, float sy, const XMFLOAT2& pos,
	UINT eNum, float eInterval, float eWidth, float eHeight, float eVel)
{
	EffectActor* effect = new EffectActor(game, imgPath, true,
		indices, lane, interval, ddsFlag, wNum, hNum, sx, sy,
		pos, ZeroVec2, 1, 0.0f);
	game->AddActor(effect);

	for (UINT i = 0; i < eNum; ++i)
	{
		float dx = -eWidth + (std::rand() / (RAND_MAX + 2.0f) * eWidth) * 2.0f;
		float dy = -eHeight + (std::rand() / (RAND_MAX + 2.0f) * eHeight) * 2.0f;
		XMFLOAT2 d(dx, dy);
		XMFLOAT2 v = Normalize(d) * eVel;

		float delay = (i + 1) * eInterval;
		effect = new EffectActor(game, imgPath, true,
			indices, lane, interval, ddsFlag, wNum, hNum, sx, sy,
			pos + d, v, 1, delay);

		game->AddActor(effect);
	}
}