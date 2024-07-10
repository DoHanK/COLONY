#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <fstream>

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	bool LoadWaveFile(const char* filename, WAVEFORMATEX& wfx, BYTE** data, DWORD& dataSize);
	bool CreateSound();
};

