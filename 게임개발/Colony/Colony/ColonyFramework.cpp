#include "stdafx.h"
#include "ColonyFramework.h"
#include "ResourceManager.h"
#include "ColonyPlayer.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//																								   //
//										This is GameFrameWork									   //	
//																								   //			 
/////////////////////////////////////////////////////////////////////////////////////////////////////
ColonyFramework::ColonyFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("Colony("));

}

ColonyFramework::~ColonyFramework(){

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

void ColonyFramework::CreateGraphicsRootSignature()
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[10];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtDetailAlbedoTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtDetailNormalTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 1; //t1: gtxtUiTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 2; //t2: gtxtUiMaskTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[15];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]);
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	GetDevice()->GetID3DDevice()->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

}

bool ColonyFramework::MakeGameObjects()
{

	GetDevice()->CommandListReset();
	CreateGraphicsRootSignature();
	GetDevice()->GetCommandList()->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	//���ҽ� �Ŵ����� �Ҹ�ɶ� ���� �Ҹ��
	Material::MakeShaders(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pd3dGraphicsRootSignature);


	////�� ���� �� �÷��̾� �� ��ü�� ���� ���ҽ��� �ε�
	m_pResourceManager = new ResourceManager(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), NULL);
	m_pUIManager = new UIManager(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pd3dGraphicsRootSignature);

	//���� �����ϴ� �Ŵ�����
	m_pSceneManager = new SceneManager(m_pResourceManager, m_pUIManager);
	m_pSceneManager->PushScene(new GamePlayScene, GetDevice(), false);
	m_pSceneManager->m_SceneStack.top()->BuildObjects(GetDevice()->GetID3DDevice(), GetDevice()->GetCommandList(), m_pResourceManager, m_pUIManager);
	



	m_pDevice->CloseCommandAndPushQueue();
	m_pDevice->WaitForGpuComplete();

	if (m_pSceneManager) m_pSceneManager->m_SceneStack.top()->ReleaseUploadBuffers();
	if (m_pResourceManager) m_pResourceManager->ReleaseUploadBuffers();
	
	//m_pSceneManager->PushScene(new GameLobbyScene, GetDevice(), true);

	return true;
}

void ColonyFramework::DestroyGameObjects()
{
	//�Ŵ��� ����

	if (m_pSceneManager) {
		delete m_pSceneManager;
	}

	if (m_pResourceManager) {
		delete m_pResourceManager;
	}
	
	if (m_pUIManager) {
		delete m_pUIManager;
	}




	//��Ʈ �ñ׳��� ������
	if (m_pd3dGraphicsRootSignature) {
		m_pd3dGraphicsRootSignature->Release();
		m_pd3dGraphicsRootSignature = NULL;
	}

	if (m_pDevice) {
		m_pDevice->DestroyDevice();
		delete m_pDevice;
		m_pDevice = nullptr;

	}

}

void ColonyFramework::AnimationGameObjects()
{


	m_ElapsedTime = m_GameTimer.GetTimeElapsed();


	if(m_pSceneManager)
		m_pSceneManager->AnimationGameObjects(m_ElapsedTime);
	if (m_pUIManager)
		m_pUIManager->AnimateUI(m_ElapsedTime);
}

void ColonyFramework::ColonyGameLoop()
{
	m_GameTimer.Tick(0.0f);
	


	//�ִϸ��̼�
	AnimationGameObjects();

	m_pDevice->CommandAllocatorReset();
	m_pDevice->CommandListReset();
	m_pDevice->MakeResourceBarrier();
	m_pDevice->RtAndDepthReset();
	//��Ʈ �ñ׳��� ���� 
	m_pDevice->GetCommandList()->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);


	//������ �ۼ�
	// ���ν� ���ε����� ���� (������ ���� ����)
	if(m_pResourceManager)
		GetDevice()->GetCommandList()->SetDescriptorHeaps(1, &m_pResourceManager->pSrvDescriptorHeap);

	if(m_pSceneManager)
		m_pSceneManager->RenderScene(GetDevice()->GetCommandList());
	
	if (m_pUIManager)
		m_pUIManager->AllLayerDrawRect(GetDevice()->GetCommandList());



	m_pDevice->CloseResourceBarrier();
	m_pDevice->CloseCommandAndPushQueue();
	m_pDevice->WaitForGpuComplete();
	m_pDevice->PresentScreen();
	m_pDevice->MoveToNextFrame();



	m_GameTimer.GetFrameRate(m_pszFrameRate + 7, 42);
	::SetWindowText(m_hWnd, m_pszFrameRate);

}


//void ColonyFramework::PlayerControlInput()
//{
//
//	static UCHAR pKeysBuffer[256];
//	float AddAcel = 0.19;
//	//�÷��̾� ���϶��� �۵��ϵ��� �����ϱ�.
//	if (GetKeyboardState(pKeysBuffer)) {
//		//�ִϸ��̼� �������Ǹ� ���� �÷��̾� ���� ����
//		DWORD dwDirection = 0;
//		DWORD dwPlayerState = STATE_IDLE;
//		
//		//Move
//		if (pKeysBuffer[W] & 0xF0)
//			dwDirection |= DIR_FORWARD;
//		if (pKeysBuffer[S] & 0xF0) 
//			dwDirection |= DIR_BACKWARD;
//		if (pKeysBuffer[A] & 0xF0) 
//			dwDirection |= DIR_LEFT;
//		if (pKeysBuffer[D] & 0xF0) 
//			dwDirection |= DIR_RIGHT;
//		 
//		//W S A D Ű�Է� �˻�
//		//RUN
//		if ((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT)) {
//			if (pKeysBuffer[L_SHIFT] & 0xF0)
//			{
//				AddAcel += PlayerRunAcel;
//				dwPlayerState = STATE_RUN;
//			}
//			else {
//				dwPlayerState = STATE_WALK;
//			}
//		}
//		 
//		//JUMP
//		if (pKeysBuffer[SPACE_BAR] & 0xF0) {
//			//����
//			dwDirection |= DIR_JUMP_UP;
//			// �÷��̾� ����
//			dwPlayerState |= STATE_JUMP;
//		}
//		// �Ѿ� ���ε�
//		if (pKeysBuffer[R] & 0xF0) {
//			// �Ѿ� ������Ʈ ����
//			
//			// �÷��̾� ����
//			dwPlayerState |= STATE_RELOAD;
//		}
//		//�ݱ�
//		if (pKeysBuffer[F] & 0xF0) {
//			// �Ѿ� ������Ʈ ����
//
//			// �÷��̾� ����
//			dwPlayerState |= STATE_PICK_UP;
//		}
//		//���� �ٲٱ�
//		if (pKeysBuffer[T] & 0xF0) {
//			// �Ѿ� ������Ʈ ����
//
//			// �÷��̾� ����
//			dwPlayerState |= STATE_SWITCH_WEAPON;
//		}
//		//�� ���
//		if (pKeysBuffer[L_MOUSE] & 0xF0) {
//
//			dwPlayerState |= STATE_SHOOT;
//		}
//	
//		//�÷��̾� �ִϸ��̼� ����
//		if(m_pPlayer)
//		((PlayerAnimationController*)(m_pPlayer->m_pSkinnedAnimationController))->SetAnimationFromInput(dwDirection, dwPlayerState);
//
//		if (m_pPlayer)
//		if (m_pPlayer->m_WeaponState == SPINE_BACK) AddAcel += NoGrapAcel;
//
//		float cxDelta = 0.0f, cyDelta = 0.0f;
//		POINT ptCursorPos;
//		static POINT m_ptOldCursorPos = {WINDOWS_POS_X + FRAME_BUFFER_WIDTH/2 , WINDOWS_POS_Y + FRAME_BUFFER_WIDTH/2 };
//			
//			////SetCursor(NULL);
//			//GetCursorPos(&ptCursorPos);
//			//cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
//			//cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
//			//SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
//			//if (m_pPlayer)
//			//m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
//			
//		if (dwDirection) if (m_pPlayer)
//			m_pPlayer->CalVelocityFromInput(dwDirection, AddAcel);
//	}
//
//
//	if (m_pPlayer)
//	m_pPlayer->UpdatePosition(m_ElapsedTime);
//}

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
		break;
	case WM_LBUTTONUP:
		if (m_pSceneManager->m_SceneStack.top()->GetType() == GameLobby) {
			m_pSceneManager->ChangeScene(new GamePlayScene, GetDevice());
		}
		else {
			m_pSceneManager->ChangeScene(new GameLobbyScene, GetDevice());
		}

		break;
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		//���콺 Ű�Է�
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	
		//Ű����Ű�Է�
		break;
	}
	return (0);
}

