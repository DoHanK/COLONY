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
	//������ũ��
	int m_nWndClientWidth;
	int m_nWndClientHeight;


	//D3Ddevice��ü
	D3Device* m_pDevice;
	//Ÿ�̸�
	ColonyTimer m_GameTimer;
	//GameScene
	Scene* m_pScene;
	//Camera
	Camera* m_pCamera;
	//������ĸ�� �����ӷ���Ʈ ǥ��
	_TCHAR m_pszFrameRate[50];

	//Manager
	ResourceManager* m_pResourceManager;


public:

	ColonyFramework();
	~ColonyFramework();
	// 3Device ��ȯ
	D3Device* GetDevice() { return m_pDevice; };
	// GameFrameWork �Լ�
	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();

	//Ű�Է�
	LRESULT CALLBACK CatchInputMessaging(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

