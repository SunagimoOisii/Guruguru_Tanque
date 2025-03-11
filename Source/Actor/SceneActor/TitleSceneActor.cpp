#include <format>

#include "Game.h"
#include "GamePad.h"
#include "TitleSceneActor.h"
#include "BlockAnimActor.h"
#include "Renderer.h"
#include "IngameSceneActor.h"

SpriteSequenceActor* TitleSceneActor::m_animSequence = nullptr;

TitleSceneActor::TitleSceneActor(Game* game)
	: SceneActor(game)
{
}

void TitleSceneActor::UpdateActor(float deltaTime)
{
	//Aボタン押下でIngameへ遷移
	auto& gamePad = m_game->GetGamePad(0);
	if (gamePad.isReleased(XINPUT_GAMEPAD_A))
	{
		//アニメーション中の場合は2回押下で遷移できるようにする
		if (m_animSequence->GetActive())
		{
			m_animSequence->ShowSprite(517);
		}
		else
		{
			SceneActor* scene = new IngameSceneActor(m_game);
			m_game->ChangeScene(scene);
		}
	}
}

void TitleSceneActor::Initialize()
{
	//初回の遷移時のみタイトルアニメーションスプライトの初期化を行う
	if (m_animSequence == nullptr)
	{
		//一連のスプライト読込
		std::array<SpriteActor*, 519> sprites{};
		XMFLOAT2 pos(m_game->GetWidth() / 2, m_game->GetHeight() / 1.125f);
		RectF scRect(m_game->GetWidth(), m_game->GetHeight() * 1.85f);
		for (int i = 0;i < sprites.size();i++)
		{
			const std::wstring index = std::format(L"{:05}", i + 1);
			const std::wstring path = { L"image\\titleScene\\titleAnim_" + index + L".jpg" };

			sprites.at(i) = new SpriteActor(m_game, path, false, false, pos,
				ZeroVec2, ZeroVec2, ZeroVec2, nullptr, &scRect);
			if (!sprites.at(i)->IsEnabled()) throw std::exception();
		}

		m_animSequence = new SpriteSequenceActor(m_game,
			std::vector<SpriteActor*>(sprites.begin(), sprites.end()), 1);
	}
	m_animSequence->SetActive(true);

	auto* soundSystem = m_game->GetSoundSystem();
	soundSystem->SetBGM(L"sound\\bgm\\title.mp3");
	soundSystem->StartBGM();
}

void TitleSceneActor::Finalize()
{
	auto* soundSystem = m_game->GetSoundSystem();
	soundSystem->StopBGM();

	m_animSequence->SetActive(false);
	m_animSequence->SetIndex(0);
	this->SetState(Actor::Dead);
}