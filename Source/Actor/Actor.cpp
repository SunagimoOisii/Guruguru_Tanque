#include "Actor.h"
#include "Component.h"
#include "Game.h"

Actor::Actor(Game* game, XMFLOAT2 pos, XMFLOAT2 vel)
	: m_game(game)
	, m_state(Active)
	, m_visible(true)
	, m_transSpeed(0.0f)
	, m_angularSpeed(0.0f)
	, m_dead(false)
{
	m_game->AddActor(this);
}

Actor::~Actor()
{
	while (!m_components.empty())
	{
		delete m_components.back();
	}
}

void Actor::Update(float deltaTime)
{
	if (m_state != Active) return;

	UpdateActor(deltaTime);
}

void Actor::UpdateActor(float deltaTime)
{
}
