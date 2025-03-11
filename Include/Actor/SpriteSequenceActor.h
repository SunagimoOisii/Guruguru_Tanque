#pragma once
#include <array>

#include "Actor.h"
#include "SpriteActor.h"

/// <summary>
/// SpriteActor*‚Ìvector‚ð—p‚¢‚Ä‹^Ž—“I‚È“®‰æ‚ð•`‰æ‚·‚é
/// </summary>
class SpriteSequenceActor : public Actor
{
public:
	SpriteSequenceActor(class Game* game, std::vector<SpriteActor*> sequence,
		int frameInterval);
	~SpriteSequenceActor();

	bool GetActive() const { return m_active; };
	void SetActive(bool flag);
	void SetIndex(int index) { m_sequenceIndex = index; };
	void SetReverse(bool isReverse) { m_reverse = isReverse; };

	void ShowSprite(int index);

	void UpdateActor(float deltaTime) override;

	void Draw() override;

private:
	std::vector<SpriteActor*> m_sequence;

	int m_frameInterval;
	int m_elapsedFrame  = 0;
	int m_sequenceIndex = 0;

	bool m_active  = false;
	bool m_reverse = false;
};