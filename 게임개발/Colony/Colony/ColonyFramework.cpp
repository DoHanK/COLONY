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

	//디바이스 초기화 및 생성 
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


	//씬 빌드 및 플레이어 및 객체들 생성 리소스들 로드
	m_pResourceManager = new ResourceManager(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), NULL);

	//씬로드
	m_pScene = new Scene;
	m_pScene->BuildObjects(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList());
	

	//카메라
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
	//카메라, 플레이어... 객체들 삭제



	//프레임 워크의 모든 객체들 파괴
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

	//애니메이션
	AnimationGameObjects();

	m_pDevice->CommandAllocatorReset();
	m_pDevice->CommandListReset();
	m_pDevice->MakeResourceBarrier();
	m_pDevice->RtAndDepthReset();

	//랜더링 작성
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
	//   마우스  VK_LBUTTON
	static UCHAR pKeysBuffer[256];
	//플레이어 씬일때만 작동하도록 설정하기.
	if (GetKeyboardState(pKeysBuffer)) {
		//애니메이션 상태정의를 위한 플레이어 상태 정의
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
		 
		//W S A D 키입력 검사
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
			//방향
			dwDirection |= DIR_JUMP_UP;
			// 플레이어 상태
			dwPlayerState |= STATE_JUMP;
		}
		// 총알 리로드
		if (pKeysBuffer[R] & 0xF0) {
			// 총알 스테이트 변경
			
			// 플레이어 상태
			dwPlayerState |= STATE_RELOAD;
		}
		//줍기
		if (pKeysBuffer[F] & 0xF0) {
			// 총알 스테이트 변경

			// 플레이어 상태
			dwPlayerState |= STATE_PICK_UP;
		}
		//총 쏘기
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
		//마우스 키입력
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		//키보드키입력
		break;
	}
	return (0);
}

