#include "SoundSource.h"

SoundSource::SoundSource(IXAudio2* xaudio2, SoundData* soundData, bool start, UINT32 loopCount)
	: m_sourceVoice(nullptr)
	, m_valid(nullptr)
	, m_state(SoundState::End)
{
	HRESULT hr = xaudio2->CreateSourceVoice(&m_sourceVoice, &(soundData->format));
	if (FAILED(hr)) return;

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData->wavData.data();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = sizeof(BYTE) * (uint32_t)soundData->wavData.size();
	m_sourceVoice->SubmitSourceBuffer(&buf);

	m_valid = true;

	if (start)
	{
		m_sourceVoice->Start();
		m_state = SoundState::Play;
	}
	else
	{
		m_state = SoundState::Pause;
	}
}

SoundSource::~SoundSource()
{
	if (!m_valid)
	{
		m_sourceVoice->Stop();

		m_sourceVoice->DestroyVoice();
		m_sourceVoice = nullptr;
	}
}

void SoundSource::Update(float deltaTime)
{
	if (!m_valid) return;

	if (m_state == SoundState::Play)
	{
		XAUDIO2_VOICE_STATE state{};
		m_sourceVoice->GetState(&state);
		if (state.BuffersQueued == 0)
		{
			m_state = SoundState::End;
		}
	}
}

void SoundSource::Start()
{
	if (!m_valid) return;

	if (m_state == SoundState::Pause)
	{
		m_sourceVoice->Start();
		m_state = SoundState::Play;
	}
}

void SoundSource::Stop()
{
	if (!m_valid) return;

	m_sourceVoice->Stop();
	m_state = SoundState::End;
}

void SoundSource::Pause()
{
	if (!m_valid) return;

	if (m_state == SoundState::Play)
	{
		XAUDIO2_VOICE_STATE state{};
		m_sourceVoice->GetState(&state);
		if (state.BuffersQueued == 0)
		{
			m_state = SoundState::End;
		}
	}
}