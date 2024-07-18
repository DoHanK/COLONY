#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	void Intialize();
	bool LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize);
	IXAudio2SourceVoice* AddSound(const char* filename);
	void Release();

	IXAudio2* pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasterVoice = nullptr;
	vector< IXAudio2SourceVoice*> m_SourceVoices;
	vector<BYTE*> m_buffers;
};

