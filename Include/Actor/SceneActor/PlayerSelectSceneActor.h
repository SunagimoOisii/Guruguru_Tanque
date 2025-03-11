#pragma once
#include "SceneActor.h"
#include "PlaneActor.h"

class PlayerSelectSceneActor : public SceneActor
{
public:
    PlayerSelectSceneActor(Game* game);

    void Initialize() override;

    void Finalize() override;

private:
    PlaneActor* background;
};