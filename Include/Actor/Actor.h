#pragma once

#include "Math.h"
#include <vector>

class Actor
{
public:
	Actor(class Game* game, XMFLOAT2 pos = ZeroVec2, XMFLOAT2 vel = ZeroVec2);
	virtual ~Actor();

	enum State
	{
		Active,
		Paused,
		Dead
	};

//セッターゲッター

	bool GetVisible() const { return m_visible; }
	void SetVisible(bool visible) { m_visible = visible; }
	float GetSpeed() const { return m_transSpeed; }
	void SetSpeed(float s) { m_transSpeed = s; }
	float GetAngularSpeed() const { return m_angularSpeed; }
	void SetAngularSpeed(float s) { m_angularSpeed = s; }
	State GetState() const { return m_state; }
	void SetState(State state) { m_state = state; }

//関数

	void Update(float deltaTime);
	virtual void UpdateActor(float deltaTime);

	virtual void Draw() = 0;

protected:
	class Game* m_game;

	bool  m_visible;
	float m_transSpeed;
	float m_angularSpeed;
	bool  m_dead;
	State m_state;
};
