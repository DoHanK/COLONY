#pragma once
#include "ColonyTimer.h"
#include "D3Device.h"

class ColonyFramework {
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	//D3Ddevice��ü
	D3Device* m_pDevice;
	//Ÿ�̸�
	ColonyTimer m_GameTimer;
	//������ĸ�� �����ӷ���Ʈ ǥ��
	_TCHAR m_pszFrameRate[50];



public:

	ColonyFramework();
	~ColonyFramework();
	// 3Device ��ȯ
	D3Device* GetDevice() { return m_pDevice; };

	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();

};

