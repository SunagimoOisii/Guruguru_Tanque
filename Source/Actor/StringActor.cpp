#include "StringActor.h"
#include "Game.h"
#include "GameUtil.h"

StringActor::StringActor(Game* game, const wchar_t* str,
	const XMFLOAT2& pos, const XMFLOAT2& vel,
	float scale, float tracking, int maxStringLength,
	const wchar_t* fontName, UINT wNum, UINT hNum, float fontSize)
	: Actor2D(game, pos, vel)
	, m_enabled(false)
	, m_fontSize(fontSize* scale)
	, m_tracking(tracking)
	, m_strLength(0)
{
	if (str == nullptr) return;

	//�Ǘ����镶�������Z�o
	size_t m_strLength = wcslen(str);
	size_t maxLen	   = m_strLength;
	if (maxStringLength > 0 &&
	   (size_t)maxStringLength > m_strLength)
	{
		maxLen = (size_t)maxStringLength;
	}
	//�Ǘ��������ŃR���e�i�v�f����ݒ�
	m_string.resize(maxLen);

	//�e������BlockSpriteActor�𐶐�
	//�ʒu��m_pos�������̍���ɂ���悤�ɒ���
	XMFLOAT2 p = pos + 0.5f * m_fontSize * (UnitVec2X + UnitVec2Y);
	for (size_t i = 0;i < m_string.size();i++)
	{
		BlockSpriteActor* actor = new BlockSpriteActor(game, fontName, true, false,
			wNum, hNum, 0, scale, scale, p);
		if (!actor->IsEnabled())
		{
			delete actor;
			ReleaseActors();
			return;
		}
		m_string[i] = actor;
		p += XMFLOAT2(m_fontSize * m_tracking, 0.0f);
	}
	SetString(str);

	m_enabled = true;
}

StringActor::~StringActor()
{
	//ReleaseActors();
}

void StringActor::SetStringPos(XMFLOAT2 pos)
{
	//�������g�̈ʒu��ݒ�
	m_pos = pos;

	//�e�����̈ʒu��ݒ�
	XMFLOAT2 p = m_pos + 0.5f * m_fontSize * (UnitVec2X + UnitVec2Y);
	for (size_t i = 0;i < m_string.size();i++)
	{
		m_string[i]->SetPos(p);
		p += XMFLOAT2(m_fontSize * m_tracking, 0.0f);
	}
}

void StringActor::UpdateActor(float deltaTime)
{
	if (!m_enabled) return;

	SetStringPos(m_pos + deltaTime * m_vel);
}

void StringActor::Draw()
{
	if (!m_enabled) return;

	for (size_t i = 0;i < m_strLength;i++)
	{
		m_string[i]->Draw();
	}
}

void StringActor::SetString(const wchar_t* str)
{
	if (str == nullptr) return;

	//�����������߁A�R���e�i�̊e�����̃C���f�b�N�X��ݒ�
	m_strLength = wcslen(str);
	if (m_strLength > m_string.size())
	{
		m_strLength = m_string.size();
	}
	for (size_t i = 0;i < m_strLength;i++)
	{
		m_string[i]->SetIndex(CalcFontimageIndex(str[i]));
	}
}

//*************************************************************
//��public ��protected
//*************************************************************

void StringActor::ReleaseActors()
{
	//�����R���e�i���
	for (size_t i = 0;i < m_string.size();i++)
	{
		delete m_string[i];
	}
	m_string.clear();
	m_enabled = false;
}