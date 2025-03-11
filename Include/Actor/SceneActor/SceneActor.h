#pragma once
#include <vector>

#include "Actor.h"
#include "SpriteSequenceActor.h"

class SceneActor : public Actor
{
public:
    SceneActor(class Game* game);
    virtual ~SceneActor() = default;

    void FadeIn();
    void FadeOut();

    virtual void Initialize();

    virtual void Finalize();

    void Draw() override;

protected:
    class Game* m_game;

private:
     static SpriteSequenceActor* m_transitionSequence;
     static bool m_isFading;

     bool m_isFinishedFade;
};