#include <cmath>
#include <exception>

#include "Game.h"

#include "TitleSceneActor.h"
#include "ModeSelectSceneActor.h"
#include "PlayerSelectSceneActor.h"
#include "StageSelectSceneActor.h"
#include "IngameSceneActor.h"
#include "ResultSceneActor.h"

Game::Game()
	: m_hwnd(nullptr)
	, m_width(0)
	, m_height(0)
	, m_startTime({})
	, m_endTime({})
	, m_freqTime({})
	, m_isRunning(false)
	, m_gamePad{0,1,2,3}
	, m_uniRand(0.0, 1.0)
	, m_fps(0.0f)
	, m_fpsVisible(true)
	, m_currentScene(nullptr)
{
	QueryPerformanceFrequency(&m_freqTime);

	//ハードウェア乱数で疑似乱数初期化
	std::random_device seed;
	m_rand = std::mt19937_64(seed());
}

Game::~Game()
{
}

double Game::GetRand()
{
	return m_uniRand(m_rand);
}

int Game::GetRand(int min, int max)
{
	return min + (int)((double)(max - min + 1) * m_uniRand(m_rand));
}

bool Game::GetBoolRand()
{
	return (GetRand(0, 1) == 1) ? false : true;
}

//*************************************************************
//↑public ↓private
//*************************************************************

void Game::Initialize(HWND hwnd, int width, int height)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	m_hwnd = hwnd;
	m_width = (width > 0) ? width : 1;
	m_height = (height > 0) ? height : 1;

	//周辺機器
	m_keyboard.Initialize();
	for (DWORD id = 0; id < 4; ++id)
	{
		m_gamePad[id].Initialize();
	}

	//レンダラー
	m_renderer = std::make_unique<Renderer>(this, 0.5f, 0.5f, 0.5f);
	if (!m_renderer->Initialize()) throw std::exception();

	//サウンドシステム起動,ファイル読み込み
	m_soundSystem = std::make_unique<SoundSystem>();
	if (!m_soundSystem->Initialize()) throw std::exception();

	m_skydome = std::make_unique<HemisphereActor>(this, Renderer::ShaderClassUnlitClamp,
		L"image\\texture\\skydome.png", true, 2500.0f, 40, 60, 960.0f, 960.0f, 740.0f,
		0.9f, true);
	if (!m_skydome->IsEnabled()) throw std::exception();

	//カメラアクター
	std::vector<const wchar_t*> path = { WHITE_TEX };
	m_cameraTarget = std::make_unique<BoxActor>(this, Renderer::ShaderClassBasicLitClamp, path, true,
		ZeroVec3, ZeroVec3, ZeroVec3, ZeroVec3, 0.0f, 0.0f, 0.0f);
	m_camera = std::make_unique<CameraActor>(this, m_cameraTarget.get(), false);
	m_camera->SetFPVCameraParam(2.0f * 2.0f * UnitVec3Z, 90.0f);
	m_camera->SetTPVCameraParam(0.1f, 10.0f, 0.1f, 0.0f);

	//フォント画像読込(ゲーム終了まで保持)
	ImageData rt = m_renderer->AllocateShaderResource(L"image\\PixelMplus12.png");
	if (rt.imgIndex == -1) throw std::exception();

	if (m_fpsVisible)
	{
		wchar_t buf[256];
		wsprintfW(buf, L"FPS:%d", (int)m_fps);
		m_fpsString = std::make_unique<StringActor>(this, buf, ZeroVec2, ZeroVec2, 2.5f, 1, 25);
		if (!m_fpsString->IsEnabled()) throw std::exception();
	}

	SceneActor* tScene = new TitleSceneActor(this);
	ChangeScene(tScene);
	m_isRunning = true;
}

bool Game::Loop()
{
	//時間計測
	float deltaTime = 0.0f;
	if (Tick(deltaTime) == false) return m_isRunning;
	
	m_fps = 1.0f / deltaTime;

	Input();
	Update(deltaTime);
	Draw();

	//deltaTimeをデバッグ情報として出力
	int mt = static_cast<int>(deltaTime);
	static wchar_t buf[1024];
	wsprintfW(buf, L"deltaTime = %d\n", mt);

	return m_isRunning;
}

bool Game::Tick(float& deltaTime)
{
	QueryPerformanceCounter(&m_endTime);
	if (m_endTime.QuadPart - m_startTime.QuadPart == 0)
	{
		return false;
	}

	deltaTime = (float)(m_endTime.QuadPart - m_startTime.QuadPart) 
		/ (float)m_freqTime.QuadPart;
	if (deltaTime < (1.0f / (FrameRate + 1.0f))) return false;

	m_startTime = m_endTime;
	static wchar_t buf[1024];
	int mt = static_cast<int>(deltaTime * 1000.0f + 0.5f);
	wsprintfW(buf, L"deltaTime[msec] = %d\n", mt);
	OutputDebugStringW(buf);
	deltaTime = (deltaTime > MaxDeltaTime) ? MaxDeltaTime : deltaTime;

	return true;
}

void Game::Input()
{
	m_keyboard.Input();
	for (DWORD id = 0; id < 4; ++id)
	{
		m_gamePad[id].Input();
	}
}

void Game::Update(float deltaTime)
{
	m_soundSystem->Update(deltaTime);
	m_renderer->Update(deltaTime);

	//テスト用
	if (m_keyboard.isReleased(0x20))
	{
		static int sceneNum = 0;
		sceneNum++;
		SceneActor* scene;
		switch (sceneNum % 3)
		{
		case 0:
			scene = new TitleSceneActor(this);
			ChangeScene(scene);
			break;

		case 1:
			scene = new IngameSceneActor(this);
			ChangeScene(scene);
			break;

		case 2:
			scene = new ResultSceneActor(this, 1);
			ChangeScene(scene);
			break;
		}
	}
	/*if (m_keyboard.isReleased(0x20))
	{
		static int sceneNum = 1;
		sceneNum++;
		SceneActor* scene;
		switch (sceneNum % 6)
		{
		case 0:
			scene = new TitleSceneActor(this);
			ChangeScene(scene);
			break;

		case 1:
			scene = new ModeSelectSceneActor(this);
			ChangeScene(scene);
			break;

		case 2:
			scene = new PlayerSelectSceneActor(this);
			ChangeScene(scene);
			break;

		case 3:
			scene = new StageSelectSceneActor(this);
			ChangeScene(scene);
			break;

		case 4:
			scene = new IngameSceneActor(this);
			ChangeScene(scene);
			break;

		case 5:
			scene = new ResultSceneActor(this);
			ChangeScene(scene);
			break;
		}
	}*/

	//アクター更新
	for (auto i = m_actors.begin(); i != m_actors.end();)
	{
		(*i)->Update(deltaTime);
		//アクター削除
		if ((*i)->GetState() == Actor::Dead)
		{
			delete* i;
			i = m_actors.erase(i);
		}
		else i++;
	}
	//一時的なアクターを追加
	for (auto actor : m_actorsTemp)
	{
		m_actors.emplace_back(actor);
	}
	m_actorsTemp.clear();

	//FPS表示の更新
	if (m_fpsVisible)
	{
		wchar_t buf[256];
		wsprintfW(buf, L"FPS:%d", (int)(m_fps + 0.5f));
		m_fpsString->SetString(buf);
		m_fpsString->Update(deltaTime);
	}
}


void Game::Draw()
{
	m_renderer->Begin();

	for (auto i = m_actors.begin();i != m_actors.end();i++)
	{
		(*i)->Draw();
	}

	m_renderer->End();
}

void Game::AddActor(Actor* actor)
{
	m_actorsTemp.emplace_back(actor);
}

void Game::ChangeScene(SceneActor* newScene)
{
	//現シーンを削除し、引数シーンを初期化する
	if (m_currentScene)
	{
		m_currentScene->Finalize();
	}
	m_currentScene = newScene;
	m_currentScene->Initialize();
}