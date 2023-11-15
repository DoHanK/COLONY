#pragma once
#include "ColonyTimer.h"
#include "D3Device.h"

class ColonyFramework {
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	//D3Ddevice객체
	D3Device* m_pDevice;
	//타이머
	ColonyTimer m_GameTimer;
	//윈도우캡션 프레임레이트 표시
	_TCHAR m_pszFrameRate[50];



public:

	ColonyFramework();
	~ColonyFramework();
	// 3Device 반환
	D3Device* GetDevice() { return m_pDevice; };

	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();

};

