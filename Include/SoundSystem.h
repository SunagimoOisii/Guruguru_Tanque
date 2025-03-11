#pragma once

#include <Windows.h>
#include <xaudio2.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <vector>
#include "GameUtil.h"
#include "SoundSource.h"
#include <string>
#include <unordered_map>

bool LoadWAVFile(const wchar_t* filePath, WAVEFORMATEX& format, std::vector<BYTE>& wavData);
bool LoadWAVFileMF(const wchar_t* filePath, WAVEFORMATEX& format, std::vector<BYTE>& wavData);

class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	bool Initialize();
	void Update(float deltaTime);

	bool LoadSoundFile(std::wstring filePath);
	bool SetBGM(std::wstring filrPath, bool start = false);
	bool StartBGM();
	bool PauseBGM();
	bool StopBGM();

	bool SetSE(std::wstring filePath);

private:
	ComPtr<IXAudio2> m_xaudio2;
	IXAudio2MasteringVoice* m_masteringVoice;
	std::unordered_map<std::wstring, SoundData*> m_soundDataList;

	static const UINT SoundEffectMaxNum = 20;
	SoundSource* m_bgm;
	SoundSource* m_ses[SoundEffectMaxNum];
};