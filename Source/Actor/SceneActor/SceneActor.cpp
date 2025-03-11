#include <iostream>

#include "SceneActor.h"
#include "Game.h"
#include "SpriteSequenceActor.h"

SpriteSequenceActor* SceneActor::m_transitionSequence = nullptr;
bool SceneActor::m_isFading = false;

SceneActor::SceneActor(Game* game)
	: Actor(game)
	, m_game(game)
{
	//シーン遷移時の暗転用SpriteSequenceを1度だけ初期化
	if (m_transitionSequence == nullptr)
	{
		//一連のスプライト読込
		std::array<SpriteActor*, 30> sprites{};
		XMFLOAT2 pos(m_game->GetWidth() / 2, m_game->GetHeight() / 1.125f);
		RectF scRect(m_game->GetWidth(), m_game->GetHeight() * 1.85f);
		for (int i = 0;i < sprites.size();i++)
		{
			const std::wstring index = std::format(L"{:05}", i + 1);
			const std::wstring path = { L"image\\fade\\Fade_" + index + L".png" };

			sprites.at(i) = new SpriteActor(m_game, path, false, false, pos,
				ZeroVec2, ZeroVec2, ZeroVec2, nullptr, &scRect);
			if (!sprites.at(i)->IsEnabled()) throw std::exception();
		}

		m_transitionSequence = new SpriteSequenceActor(m_game,
			std::vector<SpriteActor*>(sprites.begin(), sprites.end()), 1);
	}
}

void SceneActor::FadeIn()
{
	m_transitionSequence->SetReverse(true);
	m_transitionSequence->SetActive(true);
}

void SceneActor::FadeOut()
{
	m_transitionSequence->SetReverse(false);
	m_transitionSequence->SetActive(true);
}

void SceneActor::Initialize()
{
}

void SceneActor::Finalize()
{
}

void SceneActor::Draw()
{
}