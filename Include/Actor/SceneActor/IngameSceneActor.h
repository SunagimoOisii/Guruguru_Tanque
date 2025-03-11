#pragma once
#include "SceneActor.h"
#include "TerrainActor.h"

class IngameSceneActor : public SceneActor
{
public:
    IngameSceneActor(Game* game);

    void UpdateActor(float deltaTime) override;

    void Initialize() override;

    void Finalize() override;

private:
    TerrainActor* m_terrain;

    Light::Parallel* m_parallelLight;

    const int m_lightColorLoopInterval;
};