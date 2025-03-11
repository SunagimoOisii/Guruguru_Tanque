#pragma once

#include "Actor.h"

class Actor2D : public Actor
{
public:
	Actor2D(class Game* game, const XMFLOAT2& pos = ZeroVec2,
		const XMFLOAT2& vel = ZeroVec2);
	virtual ~Actor2D();

	void SetPos(const XMFLOAT2& pos) { m_pos = pos; }
	XMFLOAT2 GetPos() const { return m_pos; }
	void SetVel(const XMFLOAT2& vel) { m_vel = vel; }
	XMFLOAT2 GetVel() const { return m_vel; }

protected:
	XMFLOAT2 m_pos;
	XMFLOAT2 m_vel;
};
