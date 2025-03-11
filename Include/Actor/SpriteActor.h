#pragma once

#include "Actor2D.h"
#include <string>
#include "Renderer.h"

class SpriteActor : public Actor2D
{
public:
	SpriteActor(class Game* game,
		const std::wstring& imgPath, bool visible, bool ddsFlag = false,
		const XMFLOAT2& pos = ZeroVec2, const XMFLOAT2& vel = ZeroVec2,
		const XMFLOAT2& uvPos = ZeroVec2, const XMFLOAT2& uvVel = ZeroVec2,
		XMFLOAT2* posOffset = nullptr, RectF* scRect = nullptr, RectF* uvRect = nullptr);
	virtual ~SpriteActor();

	virtual void UpdateActor(float deltaTime) override;
	virtual void Draw() override;

	bool IsEnabled() const { return m_enabled; }
	bool IsVisible() const { return m_visible; }
	void SetVisible(bool visible) { m_visible = visible; }
	
protected:
	ImageData m_imgData;
	bool m_enabled;
	bool m_visible;

	XMFLOAT2 m_uvPos; //テクスチャの切り出し位置
	XMFLOAT2 m_uvVel; //テクスチャの切り出し速度(テクスチャアニメーション用)

	XMFLOAT2 m_posOffset; //Actor(m_pos)に対してスプライトを描画するオフセット
	RectF m_scRect; //スプライトのサイズ
	RectF m_uvRect; //テクスチャの切り出しサイズ(ピクセル単位)
};
