#pragma once

#include <Windows.h>
#include <memory>
#include <cTime>
#include <chrono>
#include <cstdlib>
#include <random>

#include "Mouse.h"
#include "GamePad.h"
#include "Keyboard.h"
#include "Renderer.h"
#include "SoundSystem.h"
#include "SpriteActor.h"

#include "BlockAnimActor.h"
#include "StringActor.h"
#include "PlaneActor.h"
#include "BoxActor.h"
#include "SphereActor.h"
#include "HemisphereActor.h"
#include "FileMeshActor.h"
#include "PlayerActor.h"
#include "ContactableActor.h"
#include "TerrainActor.h"

#include "StarSystemActor.h"
#include "RoboActor.h"
#include "BoxPersonActor.h"
#include "BulletActor.h"
#include "WallActor.h"

#include "SceneActor.h"

#include "Collider.h"

class Game
{
public:
	Game();
	~Game();

/**********************************
ゲッターセッター
**********************************/

	HWND GetHwnd() const { return m_hwnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	const Mouse& GetMouse() { return m_mouse; }

	const Keyboard& GetKeyboard() { return m_keyboard; }

	const std::unique_ptr<Renderer>& GetRenderer() const { return m_renderer; }

	/// <summary>
	/// 0以上1未満の疑似乱数値を返す
	/// </summary>
	double GetRand();

	/// <summary>
	/// min以上max以下の疑似乱数値を返す
	/// </summary>
	int GetRand(int min, int max);

	/// <summary>
	/// bool型乱数を返す
	/// </summary>
	bool GetBoolRand();

	SoundSystem* GetSoundSystem() { return m_soundSystem.get(); }

	const GamePad& GetGamePad(DWORD id) const { return m_gamePad[id]; }

	SceneActor* GetCurrentScene() const { return m_currentScene; }

	std::vector<WallActor*>& GetWalls() { return m_walls; }
	std::vector<PlayerActor*>& GetPlayers() { return m_players; }
	std::vector<Actor*>& GetActors() { return m_actors; }

/**********************************
関数
**********************************/

	void Initialize(HWND hwnd, int width, int height);

	bool Loop();

	void Input();

	void Update(float deltaTime);

	void Draw();

	void AddActor(Actor* actor);

	void ChangeScene(SceneActor* newScene);

private:
/**********************************
関数
**********************************/

	bool Tick(float& deltaTime);

/**********************************
変数
**********************************/

	static constexpr float FrameRate	= 60.0f;
	static constexpr float MaxDeltaTime = 0.05f;

	float m_fps;
	bool  m_fpsVisible;
	std::unique_ptr<StringActor> m_fpsString;

	HWND m_hwnd;
	int  m_width;
	int  m_height;

	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;
	LARGE_INTEGER m_freqTime;

	bool m_isRunning; //ゲームループを制御する

	std::mt19937_64 m_rand;
	std::uniform_real_distribution<double> m_uniRand; //0以上1未満が範囲の一様乱数として設定する

	SceneActor* m_currentScene;

	Mouse	 m_mouse;
	Keyboard m_keyboard;
	GamePad  m_gamePad[4];

	std::unique_ptr<Renderer> m_renderer;

	std::unique_ptr<SoundSystem> m_soundSystem;

/****************
描画オブジェクト
****************/

	std::unique_ptr<HemisphereActor> m_skydome;

	std::vector<Actor*> m_actors;
	std::vector<Actor*> m_actorsTemp;

	std::unique_ptr<CameraActor> m_camera;
	std::unique_ptr<BoxActor> m_cameraTarget;

	std::vector<WallActor*> m_walls;
	std::vector<PlayerActor*> m_players;
};