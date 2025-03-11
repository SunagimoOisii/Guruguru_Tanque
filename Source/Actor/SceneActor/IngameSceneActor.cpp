#include "Game.h"
#include "Renderer.h"
#include "IngameSceneActor.h"
#include "ResultSceneActor.h"

IngameSceneActor::IngameSceneActor(Game* game)
	: SceneActor(game)
	, m_terrain(nullptr)
	, m_parallelLight(nullptr)
	, m_lightColorLoopInterval(25)
{
}

void IngameSceneActor::UpdateActor(float deltaTime)
{
	static float elapsedTime = 0;
	elapsedTime += deltaTime;

	//時間経過で大域照明を朝昼夕晩の色に変化させる
	XMFLOAT3 color{};
	float timeProgress = fmod(elapsedTime / m_lightColorLoopInterval, 1.0f);
	if (timeProgress < 0.25f) //朝(オレンジ)
	{
		color = Lerp(XMFLOAT3(0.3f, 0.3f, 0.45f), XMFLOAT3(1.0f, 0.7f, 0.4f), timeProgress / 0.25f);
	}
	else if (timeProgress < 0.5f) //昼(水)
	{
		color = Lerp(XMFLOAT3(1.0f, 0.7f, 0.4f), XMFLOAT3(0.8f, 0.95f, 1.0f), (timeProgress - 0.25f) / 0.25f);
	}
	else if (timeProgress < 0.75f) //夕方(赤)
	{
		color = Lerp(XMFLOAT3(0.8f, 0.95f, 1.0f), XMFLOAT3(1.0f, 0.6f, 0.5f), (timeProgress - 0.5f) / 0.25f);
	}
	else //夜(暗い青)
	{
		color = Lerp(XMFLOAT3(1.0f, 0.6f, 0.5f), XMFLOAT3(0.3f, 0.3f, 0.45f), (timeProgress - 0.75f) / 0.25f);
	}
	m_parallelLight->color = color;
	m_game->GetRenderer()->SetParallelLight(*m_parallelLight);

	//どちらかの戦車が被弾したらResultへ遷移
	PlayerActor* player_1 = m_game->GetPlayers()[0];
	PlayerActor* player_2 = m_game->GetPlayers()[1];
	if (player_1->getHP() <= 0)
	{
		SceneActor* scene = new ResultSceneActor(m_game, 1);
		m_game->ChangeScene(scene);
	}
	else if (player_2->getHP() <= 0)
	{
		SceneActor* scene = new ResultSceneActor(m_game, 0);
		m_game->ChangeScene(scene);
	}
}

void IngameSceneActor::Initialize()
{
	//各要素の初期化を行う
	//BGM
	auto* soundSystem = m_game->GetSoundSystem();
	soundSystem->SetBGM(L"sound\\bgm\\ingame.mp3");
	soundSystem->StartBGM();

	//ライティング
	auto& renderer = m_game->GetRenderer();
	m_parallelLight = new Light::Parallel(XMFLOAT3(0.0f, -1.0f, -1.0f), XMFLOAT3(0.75f, 0.75f, 0.75f));
	renderer->SetParallelLight(*m_parallelLight);
	renderer->SetAmbientLight(OneVec3 * 0.75f);
	renderer->AllocateSpotLight(Light::Spot(XMFLOAT3(0, 0, 5), 15.0f, Normalize(XMFLOAT3(0, 0, -1)),
		360.0f, OneVec3 * 2));

	//床
	float xWidth = 100.0f, yWidth = 100.0f;
	int xNum = 21, yNum = 21;
	std::vector<float> mapHeight(xNum * yNum);
	for (int i = 0; i < yNum; i++)
	{
		for (int j = 0; j < xNum; j++)
		{
			mapHeight[j + i * xNum] = 0.0f;
		}
	}
	GridHeightMap map((float*)mapHeight.data(), xNum, yNum,
		XMFLOAT2(-0.5f * xWidth, -0.5f * yWidth), xWidth, yWidth);
	m_terrain = new TerrainActor(m_game, Renderer::ShaderClassBasicLitLoop,
		L"image\\texture\\terra-cotta-tile-light.jpg", map, 10.0f, 10.0f);
	if (!m_terrain->IsEnabled()) throw std::exception();

	//自機
	m_game->GetPlayers().push_back( new PlayerActor(m_game, Renderer::ShaderClassToon, L"model\\sensyat74.fbfix", 0,
		0.7f, 0.7f, XMFLOAT3(0.0f, -12.0f, 0.0f), ZeroVec3, XMFLOAT3(0.0f, 0.0f, 90.0f)));
	m_game->GetPlayers().push_back(new PlayerActor(m_game, Renderer::ShaderClassToon, L"model\\sensyat74.fbfix", 1,
		0.7f, 0.7f, XMFLOAT3(0.0f, 12.0f, 0.0f), ZeroVec3, XMFLOAT3(0.0f, 0.0f, -90.0f)));

	//壁
	std::vector<const wchar_t*> wallpath_mid = { L"image\\texture\\terra-cotta-tile-mid.jpg" };
	std::vector<const wchar_t*> wallpath_dark = { L"image\\texture\\terra-cotta-tile-dark.jpg" };
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ 0.0f, 17.5f,0.0f }, ZeroVec3, { 0.0f,0.0f,0.0f }, ZeroVec3, 25.0f, 0.5f, 3.0f));
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ 0.0f,-17.5f,0.0f }, ZeroVec3, { 0.0f,0.0f,0.0f }, ZeroVec3, 25.0f, 0.5f, 3.0f));
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ 10.0f,0.0f,0.0f }, ZeroVec3, { 0.0f,0.0f,90.0f }, ZeroVec3, 35.0f, 0.5f, 3.0f));
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ -10.0f,0.0f,0.0f }, ZeroVec3, { 0.0f,0.0f,90.0f }, ZeroVec3, 35.0f, 0.5f, 3.0f));

	//プレイヤーの前の壁
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ 0.0f, 8.0f,0.0f }, ZeroVec3, { 0.0f,0.0f,0.0f }, ZeroVec3, 8.0f, 0.5f, 3.0f));
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_mid,
		{ 0.0f,-8.0f,0.0f }, ZeroVec3, { 0.0f,0.0f,0.0f }, ZeroVec3, 8.0f, 0.5f, 3.0f));

	//クロス壁
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_dark,
		ZeroVec3, ZeroVec3, { 0.0f,0.0f,25.0f }, ZeroVec3, 11.0f, 0.5f, 3.0f));
	m_game->GetWalls().push_back(new WallActor(m_game, Renderer::ShaderClassToon, wallpath_dark,
		ZeroVec3, ZeroVec3, { 0.0f,0.0f,155.0f }, ZeroVec3, 11.0f, 0.5f, 3.0f));
}

void IngameSceneActor::Finalize()
{
	//各要素の破棄を行う
	//BGM
	auto* soundSystem = m_game->GetSoundSystem();
	soundSystem->StopBGM();

	//ライティング
	auto& renderer = m_game->GetRenderer();
	renderer->ResetParallelLight();
	renderer->ResetSpotLightAll();
	delete m_parallelLight;
	m_parallelLight = nullptr;

	//自機,壁,弾
	std::vector<Actor*> actors = m_game->GetActors();
	for (int i = 0; i < actors.size(); i++)
	{
		if ( typeid(*actors[i]).name() == typeid(BulletActor).name() ||
			 typeid(*actors[i]).name() == typeid(WallActor).name()   ||
			 typeid(*actors[i]).name() == typeid(PlayerActor).name() )
		{
			actors[i]->SetState(Actor::State::Dead);
		}
	}
	m_game->GetPlayers().clear();
	m_game->GetWalls().clear();

	//床
	m_terrain->SetState(Actor::Dead);

	this->SetState(Actor::Dead);
}