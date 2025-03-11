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

	//管理する文字数を算出
	size_t m_strLength = wcslen(str);
	size_t maxLen	   = m_strLength;
	if (maxStringLength > 0 &&
	   (size_t)maxStringLength > m_strLength)
	{
		maxLen = (size_t)maxStringLength;
	}
	//管理文字数でコンテナ要素数を設定
	m_string.resize(maxLen);

	//各文字のBlockSpriteActorを生成
	//位置はm_posが文字の左上にくるように調整
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
	//自分自身の位置を設定
	m_pos = pos;

	//各文字の位置を設定
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

	//文字数を求め、コンテナの各文字のインデックスを設定
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
//↑public ↓protected
//*************************************************************

void StringActor::ReleaseActors()
{
	//文字コンテナ解放
	for (size_t i = 0;i < m_string.size();i++)
	{
		delete m_string[i];
	}
	m_string.clear();
	m_enabled = false;
}