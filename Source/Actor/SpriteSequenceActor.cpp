#include "SpriteSequenceActor.h"

SpriteSequenceActor::SpriteSequenceActor(class Game* game, std::vector<SpriteActor*> sequence,
	int frameInterval)
	: Actor(game)
	, m_sequence(sequence)
	, m_frameInterval(frameInterval)
{
}

SpriteSequenceActor::~SpriteSequenceActor()
{
	for (auto* sprite : m_sequence)
	{
		sprite->SetState(Actor::Dead);
	}
}

void SpriteSequenceActor::SetActive(bool isActive)
{
	m_active = isActive;
	
	if (isActive == false)
	{
		for (auto* sprite : m_sequence)
		{
			sprite->SetVisible(false);
		}
	}
}

void SpriteSequenceActor::ShowSprite(int index)
{
	//指定インデックスのスプライトのみ表示する
	m_active = false;
	for (auto* sprite : m_sequence)
	{
		sprite->SetVisible(false);
	}
	m_sequence.at(index)->SetVisible(true);
}

void SpriteSequenceActor::UpdateActor(float deltaTime)
{
	if (!m_active) return;
	m_elapsedFrame++;

	//指定フレーム経過毎にシーケンスを1つ動かす
	if (m_elapsedFrame == m_frameInterval)
	{
		if (m_reverse) m_sequenceIndex--;
		else		   m_sequenceIndex++;

		//現インデックスのスプライトのみ表示
		if (m_sequenceIndex >= m_sequence.size() ||
			m_sequenceIndex <= 0)
		{
			m_active = false;
		}
		else
		{
			if (m_reverse)
			{
				m_sequence.at(m_sequenceIndex)->SetVisible(true);
				m_sequence.at(m_sequenceIndex + 1)->SetVisible(false);
			}
			else
			{
				m_sequence.at(m_sequenceIndex)->SetVisible(true);
				m_sequence.at(m_sequenceIndex - 1)->SetVisible(false);
			}
		}

		m_elapsedFrame = 0;
	}
}

void SpriteSequenceActor::Draw()
{
}