#include "SpriteActor.h"
#include "Game.h"

SpriteActor::SpriteActor(Game* game,
	const std::wstring& imgPath, bool visible, bool ddsFlag,
	const XMFLOAT2& pos, const XMFLOAT2& vel,
	const XMFLOAT2& uvPos, const XMFLOAT2& uvVel,
	XMFLOAT2* posOffset, RectF* scRect, RectF* uvRect)
	:Actor2D(game, pos, vel)
	, m_uvPos(uvPos)
	, m_uvVel(uvVel)
	, m_scRect()
	, m_uvRect()
	, m_posOffset(ZeroVec2)
{
//画像読み込み
	m_imgData = m_game->GetRenderer()->AllocateShaderResource(imgPath, ddsFlag);
	m_enabled = (m_imgData.imgIndex >= 0) ? true : false;
	m_visible = visible;

	if (m_enabled)
	{
		float w = static_cast<float>(m_imgData.width);
		float h = static_cast<float>(m_imgData.height);

		//sc,uvRectに指定がない場合,画像サイズを設定
		m_scRect = (scRect != nullptr) ? *scRect : RectF(w, h);
		m_uvRect = (uvRect != nullptr) ? *uvRect : RectF(w, h);
		//オフセットに指定がない場合,画像サイズの中心に一致させる
		if (posOffset != nullptr) m_posOffset = *posOffset;
		else
		{
			m_posOffset = (scRect != nullptr) ? XMFLOAT2(-scRect->width / 2.0f, -scRect->height / 2.0f)
				: XMFLOAT2(-w / 2.0f, -h / 2.0f);
		}
	}
}

SpriteActor::~SpriteActor()
{
	m_game->GetRenderer()->ReleaseShaderResource(m_imgData);
}

void SpriteActor::UpdateActor(float deltaTime)
{
	m_pos   += m_vel * deltaTime;
	m_uvPos += m_uvVel * deltaTime;
}

void SpriteActor::Draw()
{
	if (!m_enabled || !m_visible) return;
	m_game->GetRenderer()->DrawSprite(m_imgData, m_pos + m_posOffset, &m_scRect, m_uvPos, &m_uvRect);
}