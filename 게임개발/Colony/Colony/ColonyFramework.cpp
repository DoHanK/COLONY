#include "stdafx.h"
#include "ColonyFramework.h"
#include "ResourceManager.h"
#include "ColonyPlayer.h"

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
	m_pResourceManager = new ResourceManager(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), NULL);

	//���ε�
	m_pScene = new Scene;
	m_pScene->BuildObjects(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList());
	

	//ī�޶�
	m_pCamera = new Camera();
	m_pCamera->SetPosition(XMFLOAT3(0, 0,-1));
	m_pCamera->CreateShaderVariables(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList());
	m_pCamera->GenerateProjectionMatrix(1.01, 1000.f, ASPECT_RATIO, 60.f);
	m_pCamera->RegenerateViewMatrix();

	m_pPlayer = new Player();

	CLoadedModelInfo* pAngrybotModel = GameObject::LoadGeometryAndAnimationFromFile(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature(), "Model/JU_Mannequin.bin", NULL , "Model/Textures/PlayerTexture/");

	XMFLOAT3 temp = XMFLOAT3(0, 1, 0);
	m_pPlayer->Rotate(&temp ,0.f);
	m_pPlayer->SetPosition(XMFLOAT3(0, -2.0, 2));
	m_pPlayer->SetChild(pAngrybotModel->m_pModelRootObject, true);
	pAngrybotModel->m_pModelRootObject->m_pShader = new SkinnedAnimationStandardShader();
	pAngrybotModel->m_pModelRootObject->m_pShader->CreateShader(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature());
	m_pPlayer->m_pSkinnedAnimationController = new PlayerAnimationController(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), 4, pAngrybotModel);
	m_pPlayer->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	m_pPlayer->m_pSkinnedAnimationController->SetCallbackKeys(0, 0, 0);

	GameObject* bin = new GameObject();
	pAngrybotModel = GameObject::LoadGeometryAndAnimationFromFile(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature(), "Model/UMP5.bin", NULL, "Model/Textures/UMP5Texture/");
	pAngrybotModel->m_pModelRootObject->m_pShader = new StandardShader();
	pAngrybotModel->m_pModelRootObject->m_pShader->CreateShader(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(),m_pScene->GetGraphicsRootSignature());
	bin->SetChild(pAngrybotModel->m_pModelRootObject);

	temp = XMFLOAT3(0, 0, 1);
	bin->Rotate(&temp, 95.f);
	temp = XMFLOAT3(0, 1, 0);
	bin->Rotate(&temp, -18.f);
	m_pPlayer->FindFrame("RightHand")->SetChild(bin);




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
	m_pPlayer->Animate(fTimeElapsed);
	m_pScene->AnimateObjects(fTimeElapsed);
}

void ColonyFramework::ColonyGameLoop()
{
	m_GameTimer.Tick(0.0f);
	//PlayerKeyInput
	PlayerControlInput();

	//�ִϸ��̼�
	AnimationGameObjects();

	m_pDevice->CommandAllocatorReset();
	m_pDevice->CommandListReset();
	m_pDevice->MakeResourceBarrier();
	m_pDevice->RtAndDepthReset();

	//������ �ۼ�
	GetDevice()->GetCommandList()->SetDescriptorHeaps(1, &m_pResourceManager->pSrvDescriptorHeap);
	m_pScene->Render(GetDevice()->GetCommandList(),m_pCamera);

	m_pPlayer->Render(GetDevice()->GetCommandList(), m_pCamera);



	m_pDevice->CloseResourceBarrier();
	m_pDevice->CloseCommandAndPushQueue();
	m_pDevice->WaitForGpuComplete();
	m_pDevice->PresentScreen();
	m_pDevice->MoveToNextFrame();



	m_GameTimer.GetFrameRate(m_pszFrameRate + 7, 42);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}

void ColonyFramework::PlayerControlInput()
{


	// 17 Lcontrol
	// 16 Shift
	// 25 Rcontrol
	// 32 Space Bar
	//   ���콺  VK_LBUTTON
	static UCHAR pKeysBuffer[256];
	//�÷��̾� ���϶��� �۵��ϵ��� �����ϱ�.
	if (GetKeyboardState(pKeysBuffer)) {
		//�ִϸ��̼� �������Ǹ� ���� �÷��̾� ���� ����
		DWORD dwDirection = 0;
		DWORD dwPlayerState = STATE_IDLE;
		
		//Move
		if (pKeysBuffer[W] & 0xF0)
			dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[S] & 0xF0) 
			dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[A] & 0xF0) 
			dwDirection |= DIR_LEFT;
		if (pKeysBuffer[D] & 0xF0) 
			dwDirection |= DIR_RIGHT;
		 
		//W S A D Ű�Է� �˻�
		//RUN
		if ((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT)) {
			if (pKeysBuffer[L_SHIFT] & 0xF0)
			{
				dwPlayerState = STATE_RUN;
			}
			else {
				dwPlayerState = STATE_WALK;
			}
		}
		 
		//JUMP
		if (pKeysBuffer[SPACE_BAR] & 0xF0) {
			//����
			dwDirection |= DIR_JUMP_UP;
			// �÷��̾� ����
			dwPlayerState |= STATE_JUMP;
		}
		// �Ѿ� ���ε�
		if (pKeysBuffer[R] & 0xF0) {
			// �Ѿ� ������Ʈ ����
			
			// �÷��̾� ����
			dwPlayerState |= STATE_RELOAD;
		}
		//�ݱ�
		if (pKeysBuffer[F] & 0xF0) {
			// �Ѿ� ������Ʈ ����

			// �÷��̾� ����
			dwPlayerState |= STATE_PICK_UP;
		}
		//�� ���
		if (pKeysBuffer[L_MOUSE] & 0xF0) {

			dwPlayerState |= STATE_SHOOT;
		}
	

		((PlayerAnimationController*)(m_pPlayer->m_pSkinnedAnimationController))->SetAnimationFromInput(dwDirection, dwPlayerState);
		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		static POINT m_ptOldCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}
	}
}

LRESULT ColonyFramework::CatchInputMessaging(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_SIZE: {
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		//���콺 Ű�Է�
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		//Ű����Ű�Է�
		break;
	}
	return (0);
}

