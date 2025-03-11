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

	UpdateComponents(deltaTime);
	UpdateActor(deltaTime);
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : m_components)
	{
		comp->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::AddComponent(Component* component)
{
	int myOrder = component->GetUpdateOrder();
	auto i = m_components.begin();
	for (;i != m_components.end();++i)
	{
		if (myOrder < (*i)->GetUpdateOrder())
		{
			break;
		}
	}

	m_components.insert(i, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto i = std::find(m_components.begin(), m_components.end(), component);
	if (i != m_components.end())
	{
		m_components.erase(i);
	}
}