#pragma once

#include "BlockSpriteActor.h"
#include <vector>

/// <summary>
/// ������̕`��@�\������<para></para>
/// ��������̌X�̕����ɑΉ�����BlockSpriteActor�̃R���e�i���Ǘ�
/// </summary>
class StringActor : public Actor2D
{
public:
	StringActor(class Game* game, const wchar_t* str,
		const XMFLOAT2& pos = ZeroVec2, const XMFLOAT2& vel = ZeroVec2,
		float scale = 1.0f, float tracking = 1.0f, int maxStringLength = -1,
		const wchar_t* fontName = L"image\\PixelMplus12.png",
		UINT wNum = 256, UINT hNum = 90, float fontSize = 24.0f);
	virtual ~StringActor();

	void SetStringPos(XMFLOAT2 pos);
	bool IsEnabled() const { return m_enabled; }

	virtual void UpdateActor(float deltaTime) override;
	virtual void Draw() override;

	void SetString(const wchar_t* str);

protected:
	void ReleaseActors();

	bool m_enabled;
	std::vector<BlockSpriteActor*> m_string;
	size_t m_strLength; //m_string�̕`�悷�镶����
	float m_fontSize;
	float m_tracking; //���������̕����Ԋu
};
