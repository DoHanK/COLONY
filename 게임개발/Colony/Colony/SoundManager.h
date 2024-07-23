#pragma once
#include <windows.h>
#include <xaudio2.h>
#include <fstream>
#include <string>
#include <vector>
#include <mmsystem.h>
#include <dsound.h>
#include <iostream>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

using namespace std;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	bool InitDirectSound(HWND hWnd);
	LPDIRECTSOUNDBUFFER LoadWaveToBuffer(const char* fileName);
	void ChangeSceneSound();
	void Cleanup();
	void RestartSound(LPDIRECTSOUNDBUFFER sounbuffer);
	bool IsSoundBufferPlaying(LPDIRECTSOUNDBUFFER pDSBuffer);

	LPDIRECTSOUND8 g_pDS = nullptr;
	LPDIRECTSOUNDBUFFER g_pPrimaryBuffer = nullptr;
	LPDIRECTSOUNDBUFFER g_pSecondaryBuffer = nullptr;

	vector<LPDIRECTSOUNDBUFFER> m_SoundVec;
};

