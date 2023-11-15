#include "stdafx.h"
#include "ColonyFramework.h"

ColonyFramework::ColonyFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("Colony("));
}

ColonyFramework::~ColonyFramework()
{

}

bool ColonyFramework::InitD3Device(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//����̽� �ʱ�ȭ �� ���� 
	if (m_pDevice) {
		m_pDevice->WaitForGpuComplete();
		m_pDevice->DestroyDevice();
		delete m_pDevice;
		m_pDevice = nullptr;

	}

	m_pDevice = new D3Device;
	m_pDevice->CreateDevice(hInstance, hMainWnd);


	MakeGameObjects();

	return true;
}

bool ColonyFramework::MakeGameObjects()
{
	m_pDevice->CommandListReset();


	//�� ���� �� �÷��̾� �� ��ü�� ���� ���ҽ��� �ε�




	m_pDevice->CloseCommandAndPushQueue();
	m_pDevice->WaitForGpuComplete();

	return true;
}

void ColonyFramework::DestroyGameObjects()
{
	//ī�޶�, �÷��̾�... ��ü�� ����



	//������ ��ũ�� ��� ��ü�� �ı�
	if (m_pDevice) {

		m_pDevice->WaitForGpuComplete();
		m_pDevice->DestroyDevice();
		delete m_pDevice;
		m_pDevice = nullptr;
	}
}

void ColonyFramework::AnimationGameObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();



}

void ColonyFramework::ColonyGameLoop()
{
	m_GameTimer.Tick(0.0f);
	AnimationGameObjects();

	m_pDevice->CommandAllocatorReset();
	m_pDevice->CommandListReset();
	m_pDevice->MakeResourceBarrier();
	m_pDevice->RtAndDepthReset();
	// �ڵ� �κ� ����





	m_pDevice->CloseResourceBarrier();
	m_pDevice->CloseCommandAndPushQueue();
	m_pDevice->WaitForGpuComplete();
	m_pDevice->PresentScreen();
	m_pDevice->MoveToNextFrame();



	m_GameTimer.GetFrameRate(m_pszFrameRate + 7, 42);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}

