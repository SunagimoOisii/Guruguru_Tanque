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

	XMFLOAT2 m_uvPos; //�e�N�X�`���̐؂�o���ʒu
	XMFLOAT2 m_uvVel; //�e�N�X�`���̐؂�o�����x(�e�N�X�`���A�j���[�V�����p)

	XMFLOAT2 m_posOffset; //Actor(m_pos)�ɑ΂��ăX�v���C�g��`�悷��I�t�Z�b�g
	RectF m_scRect; //�X�v���C�g�̃T�C�Y
	RectF m_uvRect; //�e�N�X�`���̐؂�o���T�C�Y(�s�N�Z���P��)
};
