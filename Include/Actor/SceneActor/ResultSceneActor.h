#pragma once
#include "Game.h"
#include "SceneActor.h"
#include "SpriteSequenceActor.h"

class ResultSceneActor : public SceneActor
{
public:
    ResultSceneActor(Game* game, int winID);

    void UpdateActor(float deltaTime) override;

    void Initialize() override;

    void Finalize() override;

private:
    static SpriteSequenceActor* m_animSequence_1PWin;
    static SpriteSequenceActor* m_animSequence_2PWin;

    int m_winID;
};