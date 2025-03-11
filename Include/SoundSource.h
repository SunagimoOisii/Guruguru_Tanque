#pragma once

#include <Windows.h>
#include <xaudio2.h>
#include "GameUtil.h"

class SoundSource
{
public:
	SoundSource(IXAudio2* xaudio2, SoundData* soundData, bool start = true, UINT32 loopCount = 0);
	~SoundSource();

	void Update(float deltaTime);

	bool IsValid() const { return m_valid; }

	void Start();
	void Stop();
	void Pause();

	enum class SoundState
	{
		Play,
		Pause,
		End
	};
	SoundState GetState() const { return m_state; }

private:
	IXAudio2SourceVoice* m_sourceVoice;
	bool m_valid;
	SoundState m_state;
};