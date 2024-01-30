#include "ColonyScene.h"
#include "ColonyShader.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//							Basic Scene	-> All Scene are originated by that 					   //						
//																								   //
//						GamePlayScene Class -> This is In Game Scene							   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										LobbyScene Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GameLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature , ResourceManager* pResourceManager, UIManager* pUImanager)
{
	//pUImanager->CreateUINonNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true), NULL, NULL, 0,  TEXTUREUSE , GetType());

	UIEffectInfo EffectInfo;
	EffectInfo.ColNum = 6;
	EffectInfo.RowNum = 6;
	EffectInfo.SetTime = 0.1;
	pUImanager->CreateUISpriteNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true),
	pResourceManager->BringTexture("Model/Textures/Explosion_6x6.dds", UI_MASK_TEXTURE, true), EffectInfo, NULL, 1, (MASKUSE | TEXTUREUSE), GetType());

}




/////////////////////////////////////////////////////////////////////////////////////////////////////
//										GamePlayScene Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
GamePlayScene::GamePlayScene()
{
	
}

GamePlayScene::~GamePlayScene()
{
	
}

bool GamePlayScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		return(false);
	}
}

bool GamePlayScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	static int count = 0;

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		return(false);
	}
}

void GamePlayScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void GamePlayScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void GamePlayScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

}

void GamePlayScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 5;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 300.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.3f, 0.8f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(230.0f, 330.0f, 480.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = false;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(550.0f, 530.0f, 530.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights[4].m_bEnable = false;
	m_pLights[4].m_nType = POINT_LIGHT;
	m_pLights[4].m_fRange = 200.0f;
	m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	m_pLights[4].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[4].m_xmf3Position = XMFLOAT3(600.0f, 250.0f, 700.0f);
	m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
}


#define LOD 0
void GamePlayScene::LoadSceneObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName, const char* TexFileName,ResourceManager* pResourceManager)
{
	FILE* pFile = NULL;
	::fopen_s(&pFile, pstrFileName, "rb");
	::rewind(pFile);

	char pstrToken[256] = { '\0' };
	char pstrGameObjectName[256] = { '\0' };

	string str = "LOD";
	str += to_string(LOD);

	UINT nReads = 0;
	BYTE nStrLength = 0, nObjectNameLength = 0;

	::ReadUnityBinaryString(pFile, pstrToken, &nStrLength); //"<GameObjects>:"
	nReads = (UINT)::fread(&m_nGameObjects, sizeof(int), 1, pFile); //루트 게임 오브젝트 수 

	m_pSceneObject.reserve(m_nGameObjects);

	GameObject* pGameObject = NULL;

	int cur_object = 0;

	while (m_pSceneObject.size() < m_nGameObjects)
	{
	

		::ReadUnityBinaryString(pFile, pstrToken, &nStrLength); //"<GameObject>:"
		::ReadUnityBinaryString(pFile, pstrGameObjectName, &nObjectNameLength);
		pstrGameObjectName[nObjectNameLength] = '\0';


		::ReadUnityBinaryString(pFile, pstrToken, &nStrLength); //"<Transform>:"
		XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
		XMFLOAT4 xmf4Rotation;
		XMFLOAT4X4 mxf4x4Position;
		nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pFile);
		nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pFile); //Euler Angle
		nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pFile);
		nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pFile); //Quaternion


		::ReadUnityBinaryString(pFile, pstrToken, &nStrLength); //"<TransformMatrix>:"
		nReads = (UINT)::fread(&mxf4x4Position, sizeof(float), 16, pFile);

		if (strstr(pstrGameObjectName, str.c_str()) != nullptr) {

			pGameObject = new GameObject;
			strcpy_s(pGameObject->m_pstrFrameName, 64, pstrGameObjectName);
			pGameObject->m_xmf4x4World = mxf4x4Position;




			StandardMesh* pMesh = NULL;
			for (int j = 0; j < cur_object; j++)
			{
				if (!strcmp(pstrGameObjectName, m_pSceneObject[j]->m_pstrFrameName))
				{

					pMesh = (StandardMesh*)m_pSceneObject[j]->m_pMesh;

					pGameObject->SetMesh(pMesh);
					pGameObject->SetBoundingMesh(m_pSceneObject[j]->m_pBoundingMesh);
					pGameObject->m_nMaterials = m_pSceneObject[j]->m_nMaterials;
					pGameObject->m_ppMaterials = m_pSceneObject[j]->m_ppMaterials;

					break;
				}
			}

			if (!pMesh)
			{

				FILE* pInFile = NULL;
				char modelFilePath[256];
				strcpy_s(modelFilePath, sizeof(modelFilePath), "Model/Meshes/");
				strcpy_s(modelFilePath + 13, sizeof(modelFilePath) - 13 - 1, pstrGameObjectName);
				strcat_s(modelFilePath, sizeof(modelFilePath), ".bin");

				::fopen_s(&pInFile, modelFilePath, "rb");
				::rewind(pInFile);
				::ReadUnityBinaryString(pInFile, pstrToken, &nStrLength); //"<Mesh>:"

				pMesh = new StandardMesh(pd3dDevice, pd3dCommandList);
				pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
				pGameObject->SetMesh(pMesh);

				if (pGameObject->m_pMesh) {
					pGameObject->m_BoundingBox.Center = pGameObject->m_pMesh->GetAABBCenter();
					pGameObject->m_BoundingBox.Extents = pGameObject->m_pMesh->GetAABBExtend();
					pGameObject->m_pBoundingMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
					pGameObject->m_pBoundingMesh->AddRef();
					((BoundingBoxMesh*)pGameObject->m_pBoundingMesh)->UpdateVertexPosition(&pGameObject->m_BoundingBox);
				}

				::ReadUnityBinaryString(pInFile, pstrToken, &nStrLength); //"<Materials>:"
				pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList,NULL, pInFile,NULL,TexFileName, pResourceManager);

				::fclose(pInFile);
			}
			m_pSceneObject.push_back(pGameObject);
			cur_object += 1;
		}

	}

	::fclose(pFile);
}

void GamePlayScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature , ResourceManager* pResourceManager, UIManager* pUImanager)
{

	////카메라
	m_pCamera = new ThirdPersonCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->GenerateProjectionMatrix(1.01, 1000.f, ASPECT_RATIO, 60.f);
	m_pCamera->RegenerateViewMatrix();

	m_pGhostTraillerShader = new GhostTraillerShader();
	m_pGhostTraillerShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pGhostTraillerShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pGhostTraillerShader->AddRef();

	m_pBoundigShader = new BoundingShader();
	m_pBoundigShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pBoundigShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pBoundigShader->AddRef();

	CLoadedModelInfo* pAngrybotModel = pResourceManager->BringModelInfo("Model/JU_Mannequin.bin", "Model/Textures/PlayerTexture/");
	CLoadedModelInfo* pAngrybotModel1 = pResourceManager->BringModelInfo("Model/UMP5.bin", "Model/Textures/UMP5Texture/");

	m_pPlayer = new Player(pd3dDevice, pd3dCommandList, pAngrybotModel, pAngrybotModel1);
	m_pPlayer->SetCamera(((ThirdPersonCamera*)m_pCamera));
	m_pCamera->SetPlayer(m_pPlayer);
	m_pGameObject.reserve(400);
	for (int j = 0; j < 1; ++j) {
		for (int i = 0; i < 1; i++) {

			AlienSpider* p = new AlienSpider(pd3dDevice, pd3dCommandList, pResourceManager);
			p->SetPosition(0, 0, j);
			p->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 20);
			p->SetGhostShader(m_pGhostTraillerShader);
			m_pGameObject.push_back(p);
			
		}
	}

	LoadSceneObjectsFromFile(pd3dDevice, pd3dCommandList, "Model/Scene.bin","Model/Textures/scene/", pResourceManager);

	BuildDefaultLightsAndMaterials();


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void GamePlayScene::ReleaseObjects()
{
	ReleaseShaderVariables();

	if (m_pPlayer) m_pPlayer->Release();

	if (m_pLights) delete[] m_pLights;

	if (m_pGhostTraillerShader) m_pGhostTraillerShader->Release();
	if(m_pBoundigShader) m_pBoundigShader->Release();

	for (auto& GO : m_pGameObject) {
		GO->Release();
	}

	for (int i = 0; i < m_pSceneObject.size(); ++i) {
		m_pSceneObject[i]->Release();
	}
}

void GamePlayScene::PlayerControlInput()
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
		if (m_pPlayer)
			((PlayerAnimationController*)(m_pPlayer->m_pSkinnedAnimationController))->SetAnimationFromInput(dwDirection, dwPlayerState);

		if (m_pPlayer)
			if (m_pPlayer->m_WeaponState == SPINE_BACK) AddAcel += NoGrapAcel;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		static POINT m_ptOldCursorPos = { WINDOWS_POS_X + FRAME_BUFFER_WIDTH / 2 , WINDOWS_POS_Y + FRAME_BUFFER_WIDTH / 2 };

		//SetCursor(NULL);
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (m_pPlayer)
		m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);

		if (dwDirection) if (m_pPlayer)
			m_pPlayer->CalVelocityFromInput(dwDirection, AddAcel);
	}


	if (m_pPlayer)
		m_pPlayer->UpdatePosition(m_fElapsedTime);
}

void GamePlayScene::AnimateObjects(float fTimeElapsed)
{


	m_fElapsedTime = fTimeElapsed;

	PlayerControlInput();
	static float time = 0;
	time += m_fElapsedTime;

	for (auto& GO : m_pGameObject) {
		GO->Animate(fTimeElapsed);
		GO->SetPosition(GO->GetPosition().x + fTimeElapsed * (rand() % 4) , GO->GetPosition().y, GO->GetPosition().z + fTimeElapsed * (rand() % 4));
		//if (time > 1.7f) {
		//	time = 0;
		//	GO->m_pSkinnedAnimationController->SetTrackAnimationSet(0, rand() % 19);
		//}
	}


	m_pPlayer->Animate(fTimeElapsed);
	

}

void GamePlayScene::BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pBoundigShader->OnPrepareRender(pd3dCommandList);

	for (auto& GO : m_pGameObject) {
		GO->BoudingBoxRender(pd3dCommandList);
	}

	m_pPlayer->BoudingBoxRender(pd3dCommandList);

	for (auto& GO : m_pSceneObject) {
		GO->BoudingBoxRender(pd3dCommandList);
	}
}

void GamePlayScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{

	//카메라 초기화
	if (m_pCamera) {
		m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_pCamera->UpdateShaderVariables(pd3dCommandList);
	}


	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	for (auto& GO : m_pGameObject) {
		GO->Render(pd3dCommandList);
	}

	m_pPlayer->Render(pd3dCommandList);

	for (auto& GO : m_pSceneObject) {
		GO->Render(pd3dCommandList);
	}

	BoudingRendering(pd3dCommandList);
}

void GamePlayScene::ReleaseUploadBuffers()
{
	//Camera Player...등등.

	for (int i = 0; i <1; i++) {
		m_pGameObject[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_pSceneObject.size(); ++i) {
		m_pSceneObject[i]->ReleaseUploadBuffers();
	}



	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

}

