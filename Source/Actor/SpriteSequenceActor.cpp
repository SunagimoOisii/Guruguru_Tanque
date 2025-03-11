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
	//�w��C���f�b�N�X�̃X�v���C�g�̂ݕ\������
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

	//�w��t���[���o�ߖ��ɃV�[�P���X��1������
	if (m_elapsedFrame == m_frameInterval)
	{
		if (m_reverse) m_sequenceIndex--;
		else		   m_sequenceIndex++;

		//���C���f�b�N�X�̃X�v���C�g�̂ݕ\��
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