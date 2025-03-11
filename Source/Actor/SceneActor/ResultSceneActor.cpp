#include "Renderer.h"
#include "ResultSceneActor.h"
#include "TitleSceneActor.h"

SpriteSequenceActor* ResultSceneActor::m_animSequence_1PWin = nullptr;
SpriteSequenceActor* ResultSceneActor::m_animSequence_2PWin = nullptr;

ResultSceneActor::ResultSceneActor(Game* game, int winID)
	: SceneActor(game)
	, m_winID(winID)
{
}

void ResultSceneActor::UpdateActor(float deltaTime)
{
	//Aボタン押下でTitleへ遷移
	auto& gamePad = m_game->GetGamePad(0);
	if (gamePad.isReleased(XINPUT_GAMEPAD_A))
	{
		//アニメーション中の場合は2回押下で遷移できるようにする
		if (m_animSequence_1PWin->GetActive())
		{
			m_animSequence_1PWin->ShowSprite(272);
		}
		else if (m_animSequence_2PWin->GetActive())
		{
			m_animSequence_2PWin->ShowSprite(272);
		}
		else
		{
			SceneActor* scene = new TitleSceneActor(m_game);
			m_game->ChangeScene(scene);
		}
	}
}

void ResultSceneActor::Initialize()
{
	//初回の遷移時のみタイトルアニメーションスプライトの初期化を行う
	if (m_animSequence_1PWin == nullptr ||
		m_animSequence_2PWin == nullptr)
	{
		//勝者プレイヤーに応じたアニメの初期化
		std::array<SpriteActor*, 273> sprites_1P{};
		std::array<SpriteActor*, 273> sprites_2P{};
		XMFLOAT2 pos(m_game->GetWidth() / 2, m_game->GetHeight() / 1.125f);
		RectF scRect(m_game->GetWidth(), m_game->GetHeight() * 1.85f);

		//一連のスプライト読込
		for (int i = 0;i < sprites_1P.size();i++) //1P
		{
			const std::wstring index = std::format(L"{:05}", i + 1);
			const std::wstring path = { L"image\\result\\result_1PWin\\Result_1PWin_" + index + L".jpg" };

			sprites_1P.at(i) = new SpriteActor(m_game, path, false, false, pos,
				ZeroVec2, ZeroVec2, ZeroVec2, nullptr, &scRect);
			if (!sprites_1P.at(i)->IsEnabled()) throw std::exception();
		}
		for (int i = 0;i < sprites_2P.size();i++) //2P
		{
			const std::wstring index = std::format(L"{:05}", i + 1);
			const std::wstring path = { L"image\\result\\result_2PWin\\Result_2PWin_" + index + L".jpg" };

			sprites_2P.at(i) = new SpriteActor(m_game, path, false, false, pos,
				ZeroVec2, ZeroVec2, ZeroVec2, nullptr, &scRect);
			if (!sprites_2P.at(i)->IsEnabled()) throw std::exception();
		}

		//SpriteSequenceActorインスタンス生成
		m_animSequence_1PWin = new SpriteSequenceActor(m_game,
			std::vector<SpriteActor*>(sprites_1P.begin(), sprites_1P.end()), 1);
		m_animSequence_2PWin = new SpriteSequenceActor(m_game,
			std::vector<SpriteActor*>(sprites_2P.begin(), sprites_2P.end()), 1);
	}

	//勝者プレイヤー用のアニメを再生する
	if (m_winID == 0)	   m_animSequence_1PWin->SetActive(true);
	else if (m_winID == 1) m_animSequence_2PWin->SetActive(true);

	auto* soundSystem = m_game->GetSoundSystem();
	soundSystem->SetSE(L"sound\\se\\result.mp3");
}

void ResultSceneActor::Finalize()
{
	m_animSequence_1PWin->SetActive(false);
	m_animSequence_2PWin->SetActive(false);
	m_animSequence_1PWin->SetIndex(0);
	m_animSequence_2PWin->SetIndex(0);
	this->SetState(Actor::Dead);
}