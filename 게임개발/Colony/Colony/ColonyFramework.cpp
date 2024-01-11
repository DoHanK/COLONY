#include "stdafx.h"
#include "ColonyFramework.h"
#include "ResourceManager.h"
#include "ColonyPlayer.h"

ColonyFramework::ColonyFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("Colony("));
	ShowCursor(false);
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
	m_pCamera = new ThirdPersonCamera();
	m_pCamera->SetPosition(XMFLOAT3(0, 0,-1));
	m_pCamera->CreateShaderVariables(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList());
	m_pCamera->GenerateProjectionMatrix(1.01, 1000.f, ASPECT_RATIO, 60.f);
	m_pCamera->RegenerateViewMatrix();


	CLoadedModelInfo* pAngrybotModel = GameObject::LoadGeometryAndAnimationFromFile(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature(), "Model/JU_Mannequin.bin", NULL , "Model/Textures/PlayerTexture/");
	m_pPlayer = new Player(pAngrybotModel);
	m_pPlayer->SetCamera(((ThirdPersonCamera*)m_pCamera));
	m_pCamera->SetPlayer(m_pPlayer);
	XMFLOAT3 temp = XMFLOAT3(0, 1, 0);
	//m_pPlayer->Rotate(&temp ,00.f);
	m_pPlayer->SetPosition(XMFLOAT3(0, 0, 5));
	pAngrybotModel->m_pModelRootObject->m_pShader = new SkinnedAnimationStandardShader();
	pAngrybotModel->m_pModelRootObject->m_pShader->CreateShader(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature());
	m_pPlayer->SetAnimator(new PlayerAnimationController(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), 4, pAngrybotModel));


	GameObject* bin = new GameObject();
	pAngrybotModel = GameObject::LoadGeometryAndAnimationFromFile(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pScene->GetGraphicsRootSignature(), "Model/UMP5.bin", NULL, "Model/Textures/UMP5Texture/");
	pAngrybotModel->m_pModelRootObject->m_pShader = new StandardShader();
	pAngrybotModel->m_pModelRootObject->m_pShader->CreateShader(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(),m_pScene->GetGraphicsRootSignature());
	bin->SetChild(pAngrybotModel->m_pModelRootObject);
	m_pPlayer->SetWeapon(bin);
	


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
	m_ElapsedTime = m_GameTimer.GetTimeElapsed();
	m_pPlayer->Animate(m_ElapsedTime);
	m_pScene->AnimateObjects(m_ElapsedTime);
}

void ColonyFramework::ColonyGameLoop()
{
	m_GameTimer.Tick(0.0f);
	//PlayerKeyInput

	//�ִϸ��̼�
	AnimationGameObjects();
	PlayerControlInput();

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

#define PlayerRunAcel  0.02;
#define NoGrapAcel 0.3;
void ColonyFramework::PlayerControlInput()
{

	static UCHAR pKeysBuffer[256];
	float AddAcel = 0.19;
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
				AddAcel += PlayerRunAcel;
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
		//���� �ٲٱ�
		if (pKeysBuffer[T] & 0xF0) {
			// �Ѿ� ������Ʈ ����

			// �÷��̾� ����
			dwPlayerState |= STATE_SWITCH_WEAPON;
		}
		//�� ���
		if (pKeysBuffer[L_MOUSE] & 0xF0) {

			dwPlayerState |= STATE_SHOOT;
		}
	
		//�÷��̾� �ִϸ��̼� ����
		((PlayerAnimationController*)(m_pPlayer->m_pSkinnedAnimationController))->SetAnimationFromInput(dwDirection, dwPlayerState);

		if (m_pPlayer->m_WeaponState == SPINE_BACK) AddAcel += NoGrapAcel;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		static POINT m_ptOldCursorPos = {WINDOWS_POS_X + FRAME_BUFFER_WIDTH/2,WINDOWS_POS_Y + FRAME_BUFFER_WIDTH/2 };
			
			//SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
			m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			
		if (dwDirection) m_pPlayer->CalVelocityFromInput(dwDirection, AddAcel);
	}


	m_pPlayer->UpdatePosition(m_ElapsedTime);
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

