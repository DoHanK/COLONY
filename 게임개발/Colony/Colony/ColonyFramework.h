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
	//������ũ��
	int m_nWndClientWidth;
	int m_nWndClientHeight;


	//D3Ddevice��ü
	D3Device* m_pDevice;

	//��Ʈ�ñ׳���
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;

	//Ÿ�̸�
	ColonyTimer m_GameTimer;
	//GameScene

	//������ĸ�� �����ӷ���Ʈ ǥ��
	_TCHAR m_pszFrameRate[50];
	//�ð�
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

	//���� �Լ���
	std::deque<std::function<void(UIControlHelper&)>> m_functionQueue;

public:
	ColonyFramework();
	~ColonyFramework();


	// 3Device ��ȯ
	D3Device* GetDevice() { return m_pDevice; };
	// GameFrameWork �Լ�
	bool InitD3Device(HINSTANCE hInstance, HWND hMainWnd);
	void CreateGraphicsRootSignature();
	bool MakeGameObjects();
	void DestroyGameObjects();
	void AnimationGameObjects();
	void ColonyGameLoop();
	//void PlayerControlInput();

	//Ű�Է�
	LRESULT CALLBACK CatchInputMessaging(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

