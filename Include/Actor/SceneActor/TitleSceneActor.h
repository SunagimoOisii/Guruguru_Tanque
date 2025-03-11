#pragma once
#include <array>

#include "SceneActor.h"
#include "SpriteActor.h"
#include "SpriteSequenceActor.h"
#include "PlaneActor.h"

class TitleSceneActor : public SceneActor
{
public:
    TitleSceneActor(Game* game);

    void UpdateActor(float deltaTime) override;

    void Initialize() override;

    void Finalize() override;

private:
    static SpriteSequenceActor* m_animSequence;
};