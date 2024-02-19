#pragma once
#include "ColonyTimer.h"
#include "D3Device.h"
#include "ColonyScene.h"
#include "ColonyCamera.h"
#include "ResourceManager.h"
#include "UiManager.h"
#include "SceneManager.h"
#include "ColonyPlayer.h"
#include "UIControlHelper.h"

class ColonyFramework {
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;
	//윈도우크기
	int m_nWndClientWidth;
	int m_nWndClientHeight;


	//D3Ddevice객체
	D3Device* m_pDevice;

	//루트시그너쳐
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;

	//타이머
	ColonyTimer m_GameTimer;
	//GameScene

	//윈도우캡션 프레임레이트 표시
	_TCHAR m_pszFrameRate[50];
	//시간
	float m_ElapsedTime;
	
	//Shader
	 StandardShader*				 m_StandardShader;
	 SkinnedAnimationStandardShader* m_SkinnedAnimationShader;
	 
	//Manager
	ResourceManager*				 m_pResourceManager;
	UIManager*						 m_pUIManager;
	SceneManager*					 m_pSceneManager;
	//Helper
	UIControlHelper*				m_pUIControlHelper;

	//실행 함수들
	std::deque<std::function<void(UIControlHelper&)>> m_functionQueue;

public:
	ColonyFramework();
	~ColonyFramework();


	// 3Device 반환
	D3Device* GetDevice() { return m_pDevice; };
	// GameFrameWork 함수
	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	void CreateGraphicsRootSignature();
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();
	//void PlayerControlInput();

	//키입력
	LRESULT CALLBACK CatchInputMessaging(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

