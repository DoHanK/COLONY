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
	m_ElapsedTime = m_GameTimer.GetTimeElapsed();
	m_pPlayer->Animate(m_ElapsedTime);
	m_pScene->AnimateObjects(m_ElapsedTime);
}

void ColonyFramework::ColonyGameLoop()
{
	m_GameTimer.Tick(0.0f);
	//PlayerKeyInput

	//애니메이션
	AnimationGameObjects();
	PlayerControlInput();

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

#define PlayerRunAcel  0.02;
#define NoGrapAcel 0.3;
void ColonyFramework::PlayerControlInput()
{

	static UCHAR pKeysBuffer[256];
	float AddAcel = 0.19;
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
				AddAcel += PlayerRunAcel;
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
		//무기 바꾸기
		if (pKeysBuffer[T] & 0xF0) {
			// 총알 스테이트 변경

			// 플레이어 상태
			dwPlayerState |= STATE_SWITCH_WEAPON;
		}
		//총 쏘기
		if (pKeysBuffer[L_MOUSE] & 0xF0) {

			dwPlayerState |= STATE_SHOOT;
		}
	
		//플레이어 애니메이션 적용
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

