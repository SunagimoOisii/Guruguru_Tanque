#include "SoundSystem.h"
#include "fstream"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

struct Chunk
{
	char id[4];
};

bool LoadWAVFile(const wchar_t* filePath, WAVEFORMATEX& format, std::vector<BYTE>& waveData)
{
	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	Chunk   criff{};
	int32_t riffSize{};
	Chunk   cwave{};
	Chunk   cformat{};
	int32_t formatSize{};

	fin.read((char*)&criff, sizeof(Chunk));
	fin.read((char*)&riffSize, sizeof(int32_t));
	fin.read((char*)&cwave, sizeof(Chunk));
	fin.read((char*)&cformat, sizeof(Chunk));
	fin.read((char*)&formatSize, sizeof(int32_t));

	if (formatSize == 14)
	{
		WAVEFORMAT wformat{};
		fin.read((char*)&format, sizeof(WAVEFORMAT));
		memcpy((void*)&format, (void*)&wformat, sizeof(WAVEFORMAT));
		format.nBlockAlign = 4;
		format.cbSize      = 0;
	}
	else if (formatSize == 16)
	{
		PCMWAVEFORMAT wformat{};
		fin.read((char*)&wformat, sizeof(PCMWAVEFORMAT));
		memcpy((void*)&format, (void*)&wformat, sizeof(PCMWAVEFORMAT));
		format.cbSize = 0;
	}
	else if (formatSize == 18)
	{
		fin.read((char*)&format, sizeof(WAVEFORMATEX));
		char buf[2];
		fin.read(buf, 2);
	}
	else
	{
		return false;
	}

	Chunk   cdata;
	int32_t dataSize;
	fin.read((char*)&cdata, sizeof(Chunk));
	fin.read((char*)&dataSize, sizeof(int32_t));

	waveData.resize(dataSize);
	fin.read((char*)waveData.data(), dataSize);

	return true;
}

bool LoadWAVFileMF(const wchar_t* filePath, WAVEFORMATEX& format, std::vector<BYTE>& waveData)
{
	MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	ComPtr<IMFSourceReader> reader;
	HRESULT hr = MFCreateSourceReaderFromURL(filePath, nullptr, reader.GetAddressOf());
	if (FAILED(hr)) return false;

	ComPtr<IMFMediaType> mediaType;
	MFCreateMediaType(mediaType.GetAddressOf());
	mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mediaType.Get());
	reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, mediaType.GetAddressOf());

	WAVEFORMATEX* pformat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(mediaType.Get(), &pformat, nullptr);

	while (true)
	{
		ComPtr<IMFSample> sample;
		DWORD streamFlags = 0;
		reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr,
			&streamFlags, nullptr, sample.GetAddressOf());
		if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) break;

		ComPtr<IMFMediaBuffer> mediaBuffer;
		sample->ConvertToContiguousBuffer(mediaBuffer.GetAddressOf());

		BYTE* buffer = nullptr;
		DWORD length = 0;
		mediaBuffer->Lock(&buffer, nullptr, &length);
		waveData.resize(waveData.size() + length);
		memcpy(waveData.data() + waveData.size() - length, buffer, length);
		mediaBuffer->Unlock();
	}

	memcpy((void*)&format, (void*)pformat, sizeof(WAVEFORMATEX));
	CoTaskMemFree(pformat);
	MFShutdown();

	return true;
}

/************************************************************
↑グローバル関数　↓クラス関数
************************************************************/

SoundSystem::SoundSystem()
	: m_masteringVoice(nullptr)
	, m_bgm(nullptr)
	, m_ses{ nullptr }
{
}

SoundSystem::~SoundSystem()
{
	if (!m_bgm)
	{
		delete m_bgm;
		m_bgm = nullptr;
	}
	for (int i = 0;i < SoundEffectMaxNum;i++)
	{
		if (m_ses[i] == nullptr) continue;

		delete m_ses[i];
		m_ses[i] = nullptr;
	}

	if (!m_masteringVoice)
	{
		m_masteringVoice->DestroyVoice();
		m_masteringVoice = nullptr;
	}

	for (auto i = m_soundDataList.begin(); i != m_soundDataList.end();i++)
	{
		SoundData* s = i->second;
		delete s;
	}
	m_soundDataList.clear();
}

bool SoundSystem::Initialize()
{
	HRESULT hr = XAudio2Create(m_xaudio2.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = m_xaudio2->CreateMasteringVoice(&m_masteringVoice);
	if (FAILED(hr)) return false;

	return true;
}

void SoundSystem::Update(float deltaTime)
{
	for (int i = 0;i < SoundEffectMaxNum;i++)
	{
		if (m_ses[i] == nullptr) continue;

		m_ses[i]->Update(deltaTime);
		if (m_ses[i]->GetState() == SoundSource::SoundState::End)
		{
			delete m_ses[i];
			m_ses[i] = nullptr;
		}
	}
}

bool SoundSystem::LoadSoundFile(std::wstring filePath)
{
	auto i = m_soundDataList.find(filePath);
	if (i != m_soundDataList.end()) return true;

	size_t pathSize = filePath.size();
	if (pathSize <= 4) return false;

	size_t dotPos = filePath.find_last_of(L".");
	if (dotPos == std::wstring::npos) return false;

	std::wstring ename = filePath.substr(dotPos + 1, pathSize - dotPos - 1);
	SoundData* data = new SoundData();
	if (ename == L"wav")
	{
		if (!LoadWAVFile(filePath.c_str(), data->format, data->wavData))
		{
			delete data;
			return false;
		}
	}
	else
	{
		if(!LoadWAVFileMF(filePath.c_str(), data->format, data->wavData))
		{
			//delete data;
			//return false;
		}
	}
	m_soundDataList[filePath] = data;

	return true;
}

bool SoundSystem::SetBGM(std::wstring filePath, bool start)
{
	//すでにBGMがある場合は上書き
	if (m_bgm)
	{
		delete m_bgm;
		m_bgm = nullptr;
	}

	if (!LoadSoundFile(filePath)) return false;
	SoundData* data = m_soundDataList[filePath];
	m_bgm = new SoundSource(m_xaudio2.Get(), data, start, XAUDIO2_LOOP_INFINITE);

	return true;
}

bool SoundSystem::StartBGM()
{
	if (m_bgm == nullptr) return false;

	m_bgm->Start();
	return true;
}

bool SoundSystem::PauseBGM()
{
	if (m_bgm == nullptr) return false;

	m_bgm->Pause();
	return true;
}

bool SoundSystem::StopBGM()
{
	if (m_bgm == nullptr) return false;

	m_bgm->Stop();
	delete m_bgm;
	m_bgm = nullptr;
	return true;
}

bool SoundSystem::SetSE(std::wstring filePath)
{
	if (!LoadSoundFile(filePath)) return false;

	for (int i = 0;i < SoundEffectMaxNum;i++)
	{
		if (m_ses[i] != nullptr) continue;

		SoundData* data = m_soundDataList[filePath];
		m_ses[i] = new SoundSource(m_xaudio2.Get(), data, true, 0);
		return true;
	}

	return false;
}