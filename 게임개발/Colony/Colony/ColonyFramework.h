#pragma once
#include "ColonyTimer.h"
#include "D3Device.h"
#include "ColonyScene.h"
#include "ColonyCamera.h"
#include "ResourceManager.h"
class ColonyFramework {
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;
	//윈도우크기
	int m_nWndClientWidth;
	int m_nWndClientHeight;


	//D3Ddevice객체
	D3Device* m_pDevice;
	//타이머
	ColonyTimer m_GameTimer;
	//GameScene
	Scene* m_pScene;
	//Camera
	Camera* m_pCamera;
	//윈도우캡션 프레임레이트 표시
	_TCHAR m_pszFrameRate[50];

	//Manager
	ResourceManager* m_pResourceManager;


public:

	ColonyFramework();
	~ColonyFramework();
	// 3Device 반환
	D3Device* GetDevice() { return m_pDevice; };
	// GameFrameWork 함수
	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();

	//키입력
	LRESULT CALLBACK CatchInputMessaging(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

