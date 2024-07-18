#include "ColonyScene.h"
#include "ColonyShader.h"
#include "ColonyQuadtree.h"

#define QuadtreeDepth 1 // 1칸으로 나눠짐



string ReturnTexAddress(int num) {
	string str_m = to_string(num);
	return "Model/Textures/UITexture/number/" + str_m + ".dds";
}

UIInfo* BringUINum(UIManager* pUImanager, ResourceManager* pResourceManager, float top, float bottom, float left, float right, int num, int layer, UINT SceneType) {
	string str=ReturnTexAddress(num);
	const char* charFilename = str.c_str();
	return pUImanager->CreateUINonNormalRect(top, bottom, left, right, pResourceManager->BringTexture(charFilename, UI_TEXTURE, true),
		NULL, NULL, layer, TEXTUREUSE, SceneType,true);
}

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

void GameLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager,SoundManager* pSoundManager)
{
	//pUImanager->CreateUINonNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true), NULL, NULL, 0,  TEXTUREUSE , GetType());

	UIEffectInfo EffectInfo;
	EffectInfo.ColNum = 7;
	EffectInfo.RowNum = 7;
	EffectInfo.SetTime = 0.1f;
	pUImanager->CreateUISpriteNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/StartScreen.dds", UI_TEXTURE, true),
	pResourceManager->BringTexture("Model/Textures/Smoke45Frames.dds", UI_MASK_TEXTURE, true), EffectInfo, NULL, 0, (MASKUSE | TEXTUREUSE), GetType(),false);

	m_TbuttonF = pResourceManager->BringTexture("Model/Textures/buttonF.dds", UI_TEXTURE, true);
	m_TbuttonT = pResourceManager->BringTexture("Model/Textures/buttonT.dds", UI_TEXTURE, true);
	// 게임 스타트
	h_gameButton=pUImanager->CreateUINonNormalRect(-0.2, -0.3, -0.2, 0.2, m_TbuttonF,NULL, &UIControlHelper::GameStart, 1, TEXTUREUSE, GetType(), true);
	pUImanager->CreateUINonNormalRect(-0.2, -0.3, -0.2, 0.2, pResourceManager->BringTexture("Model/Textures/GAMESTART.dds", UI_TEXTURE, true),NULL, NULL, 2, TEXTUREUSE, GetType(), true);

	//// 세팅
	//h_settingButton=pUImanager->CreateUINonNormalRect(-0.35, -0.45, -0.2, 0.2, m_TbuttonF,NULL, NULL, 1, TEXTUREUSE, GetType(), true);
	//pUImanager->CreateUINonNormalRect(-0.35, -0.45, -0.2, 0.2, pResourceManager->BringTexture("Model/Textures/SETTINGS.dds", UI_TEXTURE, true),NULL, NULL, 2, TEXTUREUSE, GetType(), true);

	// 나가기
	h_quitButton=pUImanager->CreateUINonNormalRect(-0.35, -0.45, -0.2, 0.2, m_TbuttonF,NULL, &UIControlHelper::GameQuit, 1, TEXTUREUSE, GetType(), true);
	pUImanager->CreateUINonNormalRect(-0.35, -0.45, -0.2, 0.2, pResourceManager->BringTexture("Model/Textures/QUIT.dds", UI_TEXTURE, true),NULL, NULL, 2, TEXTUREUSE, GetType(), true);



	// -1 ~ 1 사이로
	//pUImanager->CreateUISpriteNormalRect(0, FRAME_BUFFER_HEIGHT/2, 0, FRAME_BUFFER_WIDTH/2, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true),
	//	pResourceManager->BringTexture("Model/Textures/Explosion_6x6.dds", UI_MASK_TEXTURE, true), EffectInfo, &UIControlHelper::TestFunc, 1, (MASKUSE | TEXTUREUSE), GetType(),false);

	/*SoundManager* soundManager = new SoundManager();
	IXAudio2SourceVoice* testSound=soundManager->AddSound("sample1.wav");
	testSound->Start(0);*/
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										GamePlayScene Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
GamePlayScene::GamePlayScene()
{
	m_PlayTimeTimer.Reset();
}

GamePlayScene::~GamePlayScene()
{

#ifdef WITH_MULTITHREAD

	for (int i = 0; i < MAX_THREAD_NUM; ++i) {

		m_Joblist[i].push({ END ,-1 });
	}

	for (auto& t : m_threads) {

		t.join();
	}

#endif // #WITH_MULTITHREAD
	
}

bool GamePlayScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		

		break;
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
		case '1':
			if(QuadtreeDepth>m_DepthRender)
				m_DepthRender++;
			break;
		case '2':
			if(0 < m_DepthRender)
				m_DepthRender--;
			break;
		case 'B':
			m_bBoundingRender = (m_bBoundingRender + 1) % 2;
			break;
		case 'Q':
				if (m_pLights[2].m_bEnable)	m_pLights[2].m_bEnable = false;
				else m_pLights[2].m_bEnable = true;
				break;
		case 'M':
					m_pPlayer->AddPosition(XMFLOAT3(0, 5.0F, 0));
				break;
		case 'N':
			m_pPlayer->AddPosition(XMFLOAT3(0, -5.0F, 0));
			break;
		case 'P':
			m_pPlayer->isDance = (m_pPlayer->isDance + 1) % 2;
			break;
				
		case 'L':
			for (auto& particleObject : m_pParticleObjects) {
				if (particleObject->m_bActive) {
					particleObject->m_bActive = false;
				}
				else {
					particleObject->m_bActive = true;
				}
			}
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
	m_pLights[2].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[2].m_xmf3Position.y += 2.f;
	m_pLights[2].m_xmf3Direction = m_pPlayer->GetCamera()->m_xmf3Look;




	//XMFLOAT3 dir = m_pPlayer->GetCamera()->m_xmf3Look;
	//XMFLOAT3  against  = Vector3::ScalarProduct(m_pPlayer->GetCamera()->m_xmf3Look, -1, true);

	//m_pLights[2].m_xmf3Position = m_pPlayer->GetCamera()->GetPosition();
	//m_pLights[2].m_xmf3Position.x += against.x;
	//m_pLights[2].m_xmf3Position.z += against.z;
	//m_pLights[2].m_xmf3Position.y += 1.f;
	//XMFLOAT3 LookPlayerVectore = Vector3::Subtract( m_pPlayer->GetCamera()->GetPosition(), m_pLights[2].m_xmf3Position);
	//m_pLights[2].m_xmf3Direction = LookPlayerVectore;

	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));



	::memcpy(m_pcbMappedToLightSpaces, m_pToLightSpaces, sizeof(TOLIGHTSPACES));

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbToLightGpuVirtualAddress = m_pd3dcbToLightSpaces->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(16, d3dcbToLightGpuVirtualAddress); //ToLight



}

void GamePlayScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	if (m_pd3dcbToLightSpaces) {

		m_pd3dcbToLightSpaces->Unmap(0, NULL);
		m_pd3dcbToLightSpaces->Release();

		if (m_pToLightSpaces) delete m_pToLightSpaces;

	}


}

void GamePlayScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 3;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	
	float m_DirectDiffuse = 0.5f;
	float m_DirectAmbient = 0.35f;
	float m_DirectSqercular = 0.25f;

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_fRange = 2000.0f;
	//m_pLights[0].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(m_DirectAmbient, m_DirectAmbient, m_DirectAmbient, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(m_DirectDiffuse, m_DirectDiffuse, m_DirectDiffuse, 1.0f);
	//m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(m_DirectSqercular, m_DirectSqercular, m_DirectSqercular, 1.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(-(_PLANE_WIDTH * 0.5f), _PLANE_HEIGHT, 0.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);

	

	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[1].m_fRange = 2000.0f;
	//m_pLights[1].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(m_DirectAmbient, m_DirectAmbient, m_DirectAmbient, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(m_DirectDiffuse, m_DirectDiffuse, m_DirectDiffuse, 1.0f);
	//m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(m_DirectSqercular, m_DirectSqercular, m_DirectSqercular, 1.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-(_PLANE_WIDTH * 0.5f), _PLANE_HEIGHT, 0.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);


	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights[2].m_fRange = 600.0f;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.01f, 0.01f, 0.005f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	m_pLights[2].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[2].m_xmf3Position.y += 10.0f;
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.01f, 0.01f, 0.01f);
	m_pLights[2].m_fFalloff = 15.0f;
	m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
}
#define LOD 0
void GamePlayScene::LoadSceneObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName, const char* TexFileName,ResourceManager* pResourceManager
,char* modelLocation)
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

	while (m_pSceneObject.size() <= m_nGameObjects)
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

		//if (strstr(pstrGameObjectName, str.c_str()) != nullptr) {



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

					pGameObject->m_nMaterials = m_pSceneObject[j]->m_nMaterials;
					pGameObject->m_ppMaterials = new Material * [pGameObject->m_nMaterials];
					
					for (int i = 0; i < pGameObject->m_nMaterials; ++i) {
						pGameObject->m_ppMaterials[i] = m_pSceneObject[j]->m_ppMaterials[i];
						pGameObject->m_ppMaterials[i]->AddRef();
					}

					break;
				}
			}

			if (!pMesh)
			{
				
				FILE* pInFile = NULL;
				char modelFilePath[256];
				size_t locSize = strlen(modelLocation);
				strcpy_s(modelFilePath, sizeof(modelFilePath), modelLocation);
				strcpy_s(modelFilePath + locSize, sizeof(modelFilePath) - locSize - 1, pstrGameObjectName);
				strcat_s(modelFilePath, sizeof(modelFilePath), ".bin");

				::fopen_s(&pInFile, modelFilePath, "rb");
				::rewind(pInFile);
				::ReadUnityBinaryString(pInFile, pstrToken, &nStrLength); //"<Mesh>:"

				pMesh = new StandardMesh(pd3dDevice, pd3dCommandList);
				pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
				pGameObject->SetMesh(pMesh);



				::ReadUnityBinaryString(pInFile, pstrToken, &nStrLength); //"<Materials>:"
				if (strstr(pstrGameObjectName, "Plane") != NULL) {
					pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, NULL, pInFile, m_pPlaneShader, TexFileName, pResourceManager);
					m_pScenePlane = pGameObject;
				}
				else {
					pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, NULL, pInFile, NULL, TexFileName, pResourceManager);
				}
				::fclose(pInFile);
			}

			if (pGameObject->m_pMesh) {
				pGameObject->m_BoundingBox.Center = pGameObject->m_pMesh->GetAABBCenter();
				pGameObject->m_BoundingBox.Extents = pGameObject->m_pMesh->GetAABBExtend();
				pGameObject->m_BoundingBox.Extents.x/2.0f;
				pGameObject->m_BoundingBox.Extents.z/2.0f;
				pGameObject->m_BoundingBox.Transform(pGameObject->m_BoundingBox, DirectX::XMLoadFloat4x4(&pGameObject->m_xmf4x4World));

			}

			
			m_pSceneObject.push_back(pGameObject);
			cur_object += 1;
			
		//}

	}

	::fclose(pFile);
}

void GamePlayScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager,SoundManager* pSoundManager)
{
	m_pResourceManager = pResourceManager;

	////카메라
	m_pCamera = new ThirdPersonCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->GenerateProjectionMatrix(1.01, 1000.f, ASPECT_RATIO, 60.f);
	m_pCamera->RegenerateViewMatrix();

	//쉐이더 로드
	m_pGhostTraillerShader = new GhostTraillerShader();
	m_pGhostTraillerShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pGhostTraillerShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pGhostTraillerShader->AddRef();

	m_pBoundigShader = new BoundingShader();
	m_pBoundigShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pBoundigShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pBoundigShader->AddRef();

	m_pNevMeshShader = new NevMeshShader();
	m_pNevMeshShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pNevMeshShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pNevMeshShader->AddRef();

	m_pPlaneShader = new PlaneShader();
	m_pPlaneShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pPlaneShader->AddRef();

	m_BillShader = new BillboardShader();
	m_BillShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_BillShader->AddRef();

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthSkinnedShader = new DepthSkinnedRenderingShader();
	m_pDepthSkinnedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pDepthShader = new DepthRenderingShader();
	m_pDepthShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);

	LoadSceneObjectsFromFile(pd3dDevice, pd3dCommandList, "Model/MainScene.bin","Model/Textures/scene/", pResourceManager,"Model/MainSceneMeshes/");
	//LoadSceneObjectsFromFile(pd3dDevice, pd3dCommandList, "Model/SpaceStationScene.bin", "Model/Textures/scene/", pResourceManager, "Model/SpaceStationMeshes/");

	m_pPlayer = new Player(pd3dDevice, pd3dCommandList, pResourceManager);
	m_pPlayer->SetCamera(((ThirdPersonCamera*)m_pCamera));
	m_pCamera->SetPlayer(m_pPlayer);

	m_pCollisionManager = new CollisionManager(pd3dDevice, pd3dCommandList);
	m_pCollisionManager->LoadCollisionBoxInfo(pd3dDevice, pd3dCommandList, "boundinginfo.bin");
	m_pCollisionManager->EnrollPlayerIntoCapsule(XMFLOAT3(EPSILON, 0.0, EPSILON), 0.3, 1.3, 0.3, m_pPlayer);
	m_pCollisionManager->EnrollBulletDir(m_pCamera);
	for (auto& GO : m_pSceneObject) {
		GO->m_BoundingBox;
		if ((strstr(GO->m_pstrFrameName, "SM_Mountain_A") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Mountain_B") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Plant_A") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Plant_B") == NULL
			&& strstr(GO->m_pstrFrameName, "SF_Free-Fighter") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_LongGrass") == NULL)) {

			m_pCollisionManager->EnrollObjectIntoBox(false, GO->m_BoundingBox.Center, GO->m_BoundingBox.Extents, GO);

		}
	}


	//Octree Crate
	XMFLOAT3 OctreeScale = m_pScenePlane->m_BoundingBox.Extents;
	OctreeScale = Vector3::ScalarProduct(OctreeScale, 1.2f, false);

	OctreeScale.y = 20.f;
	XMFLOAT3 OctreeCenter = XMFLOAT3(-1.0, 0, 0);


	OctreeCenter.y = 20.f;
	m_pQuadTree = new QuadTree(pd3dDevice, pd3dCommandList, 0, OctreeCenter, OctreeScale);
	m_pQuadTree->BuildTreeByDepth(pd3dDevice, pd3dCommandList, QuadtreeDepth);




	//auto start_t = std::chrono::high_resolution_clock::now();
	////코드 기입
	//auto end_t = std::chrono::high_resolution_clock::now();
	//auto exec_t = end_t - start_t;
	//auto exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>(exec_t).count();
	//DebugValue::Printfloat("실행시간: ", exec_ms);

#ifdef WITH_MULTITHREAD	

	m_pQuadTree->BringDepthTrees(m_Quadlist, QuadtreeDepth);
	std::sort(m_Quadlist.begin(), m_Quadlist.end(), [](QuadTree* a, QuadTree* b) { return a->m_SameDepthidx < b->m_SameDepthidx; });
	//m_Quadlist.sort([](QuadTree* a, QuadTree* b) { return a->m_SameDepthidx < b->m_SameDepthidx; });


	{
		//vector<thread> threads;
		//인덱스 정렬.
		int count = 0;
		for (auto& QT : m_Quadlist) {

			//threads.push_back(thread(&QuadTree::InsertStaticObject, QT, m_pSceneObject));
			QT->InsertStaticObject(m_pSceneObject);   // 싱글코어 코드
			QT->SettingStaticBounding(*m_pCollisionManager);
			QT->m_pCamera = m_pCamera;
			QT->m_pPlayer = m_pPlayer;
			m_threads.push_back(thread(&GamePlayScene::ThreadWorker, this, count++));
		}


	}

#endif


	m_pNevMeshBaker = new NevMeshBaker(pd3dDevice, pd3dCommandList, CELL_SIZE, H_MAPSIZE_X, H_MAPSIZE_Y ,true);
	//m_pNevMeshBaker->BakeNevMeshByCollision(pd3dDevice, pd3dCommandList,m_pCollisionManager->m_StaticObjects);
	//m_pNevMeshBaker->SaveNevMesh();
	//m_pNevMeshBaker->LoadNevMesh();

	m_pPathFinder = new PathFinder();
	m_pPathFinder->BuildGraphFromCell(m_pNevMeshBaker->m_Grid, m_pNevMeshBaker->m_WidthCount, m_pNevMeshBaker->m_HeightCount);

	m_pskybox = new SkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pskybox->AddRef();

	m_pPerceptionRangeMesh = new PerceptionRangeMesh(pd3dDevice, pd3dCommandList);
	//Monster Create
	Texture* spiderColor[7];
	int i = 0;
	spiderColor[i++] = pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Bone_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_DarkGreen_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Green_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_purple_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Red_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_White_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[6] =pResourceManager->BringTexture("Model/Textures/GhostMask1.dds", DETAIL_NORMAL_TEXTURE, true);

	m_pGameObject.reserve(400);
	for (int j = 0; j < 100; ++j) {
		for (int i = 0; i < 1; i++) {
			int idex = m_pPathFinder->GetInvalidNode();
			AlienSpider* p = new AlienSpider(pd3dDevice, pd3dCommandList, pResourceManager, m_pPathFinder, MonsterSizeDis(gen));
			p->SetPosition(m_pPathFinder->m_Cell[idex].m_BoundingBox.Center.x, 15.f, m_pPathFinder->m_Cell[idex].m_BoundingBox.Center.z);
			//p->SetPosition(j, 0.f, 0.f);
			p->SetPerceptionRangeMesh(m_pPerceptionRangeMesh);
			p->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (Range_2+j) % AlienAnimationName::EndAnimation);
			p->SetGhostShader(m_pGhostTraillerShader);
			if (p->m_MonsterScale > 6.f) {

				p->m_pSpiderTex = spiderColor[4];
			}
			else
			p->m_pSpiderTex = spiderColor[j % 6];
			p->m_pGhostMaskTex = spiderColor[6];
			m_pCollisionManager->EnrollEnemy(p);
			m_pGameObject.push_back(p);
		}
	}
	
	m_pTestBox = new ShphereMesh(pd3dDevice, pd3dCommandList,20,20, PlayerRange);
	
	//billboard -> doAnimate,active,ownerObject,TickAddPosition 설정
	for (int i = 0; i < 29; i++) {

		string file = "Model/Textures/BloodTexture/";
		if (i < 15) {
			file += "Front_";
			file += to_string(i + 1);
			file += ".dds";
		}
		else {
			file += "Side_";
			file += to_string(i - 14);
			file += ".dds";
		}

		for (int j = 0; j < 10; ++j) {
			Billboard* pBillObject = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
				pResourceManager->BringTexture(file.c_str(), BILLBOARD_TEXTURE, true), m_BillShader, NULL);
			pBillObject->doAnimate = true;
			pBillObject->SetAddPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
			if (i == 5 || i == 9 || i == 11 || i == 28) {
				pBillObject->SetRowNCol(8, 16);

			}
			else if (i == 15 || i == 16 || i == 17 || i == 19 || i == 20 || i == 21) {

				pBillObject->SetRowNCol(16, 8);
			}
			else {
				pBillObject->SetRowNCol(8, 8);
			}

			pBillObject->SetOffsetPos(XMFLOAT3(0, -0.1f, 0));
			pBillObject->m_OffsetPos = XMFLOAT3(2.0f, 1.7f, 2.0f);

			pBillObject->m_BillMesh->UpdataVertexPosition(UIRect(2.0, -2.0, -2.0, 2.0), 0.0f);
			pBillObject->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
			pBillObject->SettedTimer = 0.005f;
			pBillObject->AddRef();
			m_pBloodBillboard[i].push_back(pBillObject);
		}
	}


	//billboard test
	m_pBillObject = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
	pResourceManager->BringTexture("Model/Textures/Explosion03.dds", BILLBOARD_TEXTURE, true), m_BillShader,m_pPlayer->m_SelectWeapon.FindFrame("Export"));
	m_pPlayer->m_pExportEffect = m_pBillObject;
	m_pBillObject->doAnimate = true;
	m_pBillObject->active = false;
	m_pBillObject->SetRowNCol(4, 4);
	m_pBillObject->m_BillMesh->UpdataVertexPosition(UIRect(0.15, -0.15, -0.15, 0.15), 1.0f);
	m_pBillObject->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
	m_pBillObject->SettedTimer = 0.01f;
	m_pBillObject->doOnce = true;
	m_pBillObject->AddRef();

	



	//// particle
	m_pParticleShader = new ParticleShader();
	m_pParticleShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pParticleShader->AddRef();

	for (int i = 0; i <400; i < i++) {
		ParticleObject* pParticleObject = new ParticleObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pResourceManager->BringTexture("Model/Textures/Raindrop2.dds", PARTICLE_TEXTURE, true), m_pParticleShader,
			XMFLOAT3(GetRandomFloatInRange(-250.f,250.f), 0.0f, GetRandomFloatInRange(-250.f, 250.f)), 0, XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), GetRandomFloatInRange(8.0f, 12.0f), GetRandomFloatInRange(5.0f, 10.0f), 0.0f, 0.0f, 100);
		m_pParticleObjects.push_back(pParticleObject);
	}

	
	
	
	CLoadedModelInfo*  bullet = pResourceManager->BringModelInfo("Model/weapon/BulletCasing.bin", NULL);
	bulletcasings.reserve(60); 
	for (int i = 0; i < 60; i++) {
		BulletCasing* pbulletcasing = new BulletCasing;
		pbulletcasing->m_bActive = false;
		pbulletcasing->SetChild(bullet->m_pModelRootObject, true);
		m_pCollisionManager->EnrollbulletIntoBox(false, pbulletcasing->GetPosition(),
			bullet->m_pModelRootObject->m_pMesh->GetAABBExtend(), bullet->m_pModelRootObject->m_xmf4x4ToParent, pbulletcasing);

		bulletcasings.push_back(pbulletcasing);
	}

	//RedZone
	m_RedZoneShader = new RedZoneShader();
	m_RedZoneShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_RedZoneShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_RedZoneShader->AddRef();

	m_RedZone = new RedZone(pd3dDevice,pd3dCommandList,pd3dGraphicsRootSignature, "Model/RedZone2.bin", m_RedZoneShader, "Model/Textures/",pResourceManager);
	m_pCollisionManager->EnrollRedZoneIntoSphere(m_RedZone->RedZoneObjectInfo->m_pModelRootObject->GetPosition(), 50.f, m_RedZone);

	//RedZoneEffect
	m_pRedZoneEffect = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		pResourceManager->BringTexture("Model/Textures/RedZoneExplosion.dds", BILLBOARD_TEXTURE, true), m_BillShader, m_RedZone->RedZoneObjectInfo->m_pModelRootObject);
	m_pRedZoneEffect->doAnimate = true;
	m_pRedZoneEffect->active = false;
	m_pRedZoneEffect->SetRowNCol(8, 8);
	m_pRedZoneEffect->m_BillMesh->UpdataVertexPosition(UIRect(50.0, -50.0, -50.0, 50.0), 1.0f);
	m_pRedZoneEffect->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
	m_pRedZoneEffect->SettedTimer = 0.01f;
	m_pRedZoneEffect->doOnce = true;
	m_pRedZoneEffect->AddRef();


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//item
	CLoadedModelInfo* itemBoxInfo = pResourceManager->BringModelInfo("Model/Item/itemBox.bin", "Model/Textures/Item/");
	/*CLoadedModelInfo* rifleInfo = pResourceManager->BringModelInfo("Model/Item/rifle.bin", "Model/Textures/Item/");
	CLoadedModelInfo* shotgunInfo = pResourceManager->BringModelInfo("Model/Weapon/shotgun.bin", "Model/Textures/Item/");
	CLoadedModelInfo* machinegunInfo = pResourceManager->BringModelInfo("Model/Weapon/machinegun.bin", "Model/Textures/Item/");
	CLoadedModelInfo* syringeInfo = pResourceManager->BringModelInfo("Model/Item/syringe.bin", "Model/Textures/Item/");
	CLoadedModelInfo* eyeInfo = pResourceManager->BringModelInfo("Model/Item/eye.bin", "Model/Textures/Item/");*/

	for (int i = 0; i < 20; i++) {
		GameObject* pitemBox = new ItemObject();
		pitemBox->SetChild(itemBoxInfo->m_pModelRootObject, true);
		int index = m_pPathFinder->GetInvalidNode();
		pitemBox->Rotate(-90.0f, GetRandomFloatInRange(-90.0f, 90.0f), 0.0f);
		pitemBox->m_BoundingBox.Extents = XMFLOAT3(0, 0, 0);
		pitemBox->MergehierarchyBoundingBox(pitemBox->m_BoundingBox);
		pitemBox->SetPosition(m_pPathFinder->m_Cell[index].m_BoundingBox.Center.x, 0.f, m_pPathFinder->m_Cell[index].m_BoundingBox.Center.z);
		pitemBox->UpdateBoundingBox(pd3dDevice, pd3dCommandList);
		m_pCollisionManager->EnrollItemIntoBox(pitemBox->m_BoundingBox.Center, pitemBox->m_BoundingBox.Extents, itemBoxInfo->m_pModelRootObject->m_xmf4x4ToParent, pitemBox);
		pitemBox->m_bActive = true;
		m_itemBoxes.push_back(pitemBox);
	}

	//ItemBoxExplosion
	m_ItemBoxExplosion = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		pResourceManager->BringTexture("Model/Textures/ItemBoxExplosion.dds", BILLBOARD_TEXTURE, true), m_BillShader, NULL);
	m_ItemBoxExplosion->doAnimate = true;
	m_ItemBoxExplosion->active = false;
	m_ItemBoxExplosion->SetRowNCol(1, 13);
	m_ItemBoxExplosion->m_BillMesh->UpdataVertexPosition(UIRect(3.0, -1.0, -2.0, 2.0), 1.0f);
	m_ItemBoxExplosion->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
	m_ItemBoxExplosion->SettedTimer = 0.02f;
	m_ItemBoxExplosion->doOnce = true;
	m_ItemBoxExplosion->AddRef();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ShotGunEffect 
	m_pShotgunEffect = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		pResourceManager->BringTexture("Model/Textures/ShotgunEffect.dds", BILLBOARD_TEXTURE, true), m_BillShader, m_RedZone->RedZoneObjectInfo->m_pModelRootObject);
	m_pShotgunEffect->doAnimate = true;
	m_pShotgunEffect->active = false;
	m_pShotgunEffect->SetRowNCol(3,5);
	m_pShotgunEffect->m_BillMesh->UpdataVertexPosition(UIRect(50.0, -50.0, -50.0, 50.0), 1.0f);
	m_pShotgunEffect->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
	m_pShotgunEffect->SettedTimer = 0.01f;
	m_pShotgunEffect->doOnce = true;
	m_pShotgunEffect->AddRef();
	


	BulidUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pResourceManager, pUImanager);
	BuildDefaultLightsAndMaterials();
	BuildDepthTexture(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


float GamePlayScene::GetRandomFloatInRange(float minVal, float maxVal)
{

	float random01 = static_cast<double>(rand()) / RAND_MAX; // 0.0부터 1.0 사이의 랜덤 값

	// 범위를 조절하여 원하는 범위 내의 랜덤 값 생성
	float randomInRange = minVal + random01 * (maxVal - minVal);

	return randomInRange;

}

void GamePlayScene::AnimateObjectsWithMultithread(float fTimeElapsed)
{




	if (m_bisCameraShaking) {
		//intensity, duration
		m_pCamera->UpdateCameraShake(5.0f, 0.3f, fTimeElapsed, m_pPlayer->m_gunType);
	}



	m_fElapsedTime = fTimeElapsed;
	m_pPlayer->m_ReloadTime += fTimeElapsed;
	PlayerControlInput();
	m_pCollisionManager->CollisionPlayerToStaticObeject();
	m_pCollisionManager->CollisionPlayerToItemBox();
	m_pCollisionManager->CollisionPlayerToEnemy();

	m_bCrashRedZone = m_pCollisionManager->CollisionPlayerToRedZone();

	readycount = MAX_THREAD_NUM;

	for (int i = 0; i < MAX_THREAD_NUM; ++i) {
		m_Joblist[i].push({ ANIMATION, -1 });
	}







	m_RedZoneHurt += fTimeElapsed;
	if (m_RedZoneHurt > 5.0f) {
		m_RedZoneHurt = 0.0f;
		if (m_bCrashRedZone && m_pPlayer->m_HP > 0) {
			m_pPlayer->m_HP -= 1;
			m_isHurt = true;
		}
	}



	if (m_isHurt) {
		m_hurtAnimation += fTimeElapsed;
		if (m_hurtAnimation > 0.2f) {
			m_isHurt = false;
			m_hurtAnimation = 0.0f;
		}
	}


	m_pPlayer->Animate(fTimeElapsed);

	for (auto& GO : m_pBillObjects) {
		if (GO->doAnimate) {
			GO->Animate(fTimeElapsed);
		}
	}

	m_pBillObject->Animate(fTimeElapsed);

	if (m_pRedZoneEffect->doAnimate) {
		m_pRedZoneEffect->Animate(fTimeElapsed);
	}

	if (m_ItemBoxExplosion->doAnimate) {
		m_ItemBoxExplosion->Animate(fTimeElapsed);
	}

	for (int i = 0; i < 29; ++i) {
		for (auto& B : m_pBloodBillboard[i]) {
			if (B->doAnimate) {
				B->Animate(fTimeElapsed);
			}
		}
	}
	//crashUIAnimation
	if (m_bcrashOk) {
		m_crashAnimation += fTimeElapsed;
		if (m_crashAnimation > 0.3f) {
			m_bcrashOk = false;
			m_crashAnimation = 0.0f;
		}
	}
	// 총알 
	for (auto& b : bulletcasings) {
		if (b->m_bActive)
			b->Update(fTimeElapsed);
	}

	m_pCollisionManager->CollisionBulletToObject();



	m_pPlayer->m_xmf3FinalPosition = m_pPlayer->m_xmf3Position;



	while (readycount != 0);






}

void GamePlayScene::RenderWithMultiThread(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12GraphicsCommandList* pd3dSubCommandList[], int ableThread, Camera* pCamera)
{

	if (true) {

		float LifeTime = 20.0f;
		TotalPlayTime = static_cast<int>(m_PlayTimeTimer.GetTotalTime());
		m_currentMinute = static_cast<int>(TotalPlayTime / LifeTime);
		//쉐이더로 전체 시간 보내기
		float totaltime = m_PlayTimeTimer.GetTotalTime();
		//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &totaltime, 41);
		float bredzone = m_bCrashRedZone;
		// 플레이어 방사능에 있는지 여부...
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &bredzone, 40);
		//속도에 따른 블러링

		//if (m_pPlayer) {
		//	XMFLOAT3 vel = m_pPlayer->GetVelocity();
		//	float velocity = sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
		//	int	 velo = int(velocity);
		//	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &velo, 39);
		//}
		



		//카메라 초기화
		if (m_pCamera) {
			m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
			m_pCamera->UpdateShaderVariables(pd3dCommandList);
		}


		UpdateShaderVariables(pd3dCommandList);

		D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

		for (int i = 0; i < ableThread; ++i) {
			if (m_pCamera) {
				m_pCamera->SetViewportsAndScissorRects(pd3dSubCommandList[i]);
				m_pCamera->UpdateShaderVariables(pd3dSubCommandList[i]);
			}


			UpdateShaderVariables(pd3dSubCommandList[i]);

			D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
			pd3dSubCommandList[i]->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

			pd3dSubCommandList[i]->SetGraphicsRoot32BitConstants(1, 1, &bredzone, 40);
		}


		if (true) {


			readycount = MAX_THREAD_NUM;

			for (int i = 0; i < MAX_THREAD_NUM; ++i) {
				long long addr = reinterpret_cast<long long>(pd3dSubCommandList[i]);
				m_Joblist[i].push({ RENDERING, addr });
			}

		}

		m_pskybox->Render(pd3dCommandList, m_pPlayer->GetCamera(), m_pPlayer);
		m_pPlayer->Render(pd3dCommandList);



		if (m_bBoundingRender) BoudingRendering(pd3dCommandList);

		for (auto& b : bulletcasings) {
			if (b->m_bActive) {
				b->UpdateTransform();
				b->Render(pd3dCommandList);

			}
		}

		for (auto& GO : m_pBillObjects) {
			if (GO->active) {
				GO->Render(pd3dCommandList, m_pPlayer->GetCamera());
			}
		}


		if (m_pBillObject->active) {
			m_pBillObject->Render(pd3dCommandList, m_pPlayer->GetCamera());
		}


		for (int i = 0; i < 29; ++i) {
			for (auto& B : m_pBloodBillboard[i]) {
				if (B->active) {
					B->Render(pd3dCommandList, m_pPlayer->GetCamera());
				}
			}
		}

		for (auto& ParticleObject : m_pParticleObjects) {
			if (ParticleObject->m_bActive) {
				ParticleObject->Render(pd3dCommandList);
			}
		}


		if (m_RedZone) {

			if (m_currentMinute > m_LastMinute) {

				m_pRedZoneEffect->SetPosition(m_RedZone->GetPosition());
				m_RedZone->m_xmf4x4ToParent = Matrix4x4::Identity();
				int RandomPosition = GetRandomFloatInRange(-200.f, 200.f);
				m_RedZone->SetPosition(RandomPosition, 0, RandomPosition);
				m_RedZone->m_prexmf4x4ToParent = m_RedZone->m_xmf4x4ToParent;
				m_LastMinute = m_currentMinute;
				m_pRedZoneEffect->active = true;

			}

			if (TotalPlayTime % int(LifeTime) == int(LifeTime - 1)) {

				m_RedZone->m_xmf4x4ToParent = m_RedZone->m_prexmf4x4ToParent;
				float size = m_PlayTimeTimer.GetTotalTime() - int(m_PlayTimeTimer.GetTotalTime());
				size = 1.0f - size;
				m_RedZone->SetScale(size, size, size);

			}

	
		}

		if (m_pRedZoneEffect->active) {
			m_pRedZoneEffect->NoSetPositionRender(pd3dCommandList, m_pPlayer->GetCamera());
		}


		for (auto& GO : m_itemBoxes) {
			if (GO->m_bActive) {
				GO->UpdateTransform(NULL);
				GO->Render(pd3dCommandList);
			}
		}


		if (m_ItemBoxExplosion->active) {
			m_ItemBoxExplosion->Render(pd3dCommandList, m_pPlayer->GetCamera());
		}



		while (readycount != 0);

		if (m_RedZone) m_RedZone->Render(pd3dSubCommandList[MAX_THREAD_NUM -1]);
	}
}

void GamePlayScene::ThreadWorker(int threadnum)
{


	while (true) {

		if (!m_Joblist[threadnum].empty()) {

			pair<int, long long > OP;


			while (m_Joblist[threadnum].try_pop(OP));

			switch (OP.first) {
			case ANIMATION: {


				m_Quadlist[threadnum]->AnimateObjects(m_fElapsedTime, m_pGameObject);

				while (true) {
					int precount = readycount;
					int nowcount = precount - 1;
					if (CAS(&readycount, precount, nowcount)) break;
				}





				break;
			}
			case RENDERING: {
				ID3D12GraphicsCommandList* subcommandlist = reinterpret_cast<ID3D12GraphicsCommandList*>(OP.second);
				m_Quadlist[threadnum]->Render(subcommandlist);
				while (true) {
					int precount = readycount;
					int nowcount = precount - 1;
					if (CAS(&readycount, precount, nowcount)) break;

				}

				break;
			}
			case END:
				return;

			}




		}


	}

}

void GamePlayScene::BulidUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager)
{////UI

	// None
	m_TNone = pResourceManager->BringTexture("Model/Textures/None.dds", UI_TEXTURE, true);

	//Scope
	m_TscopeShoot = m_pResourceManager->BringTexture("Model/Textures/UITexture/ScopeShoot.dds", UI_TEXTURE, true);
	m_TscopeShoot2 = m_pResourceManager->BringTexture("Model/Textures/UITexture/ScopeShoot2.dds", UI_TEXTURE, true);
	h_scopeMode = pUImanager->CreateUINonNormalRect(1.0, -1.0,-1.0, 1.0, m_TNone, NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	//BloodScreen
	m_TbloodScreen = pResourceManager->BringTexture("Model/Textures/UITexture/BloodScreen.dds", UI_TEXTURE, true);
	h_HurtState = pUImanager->CreateUINonNormalRect(1.0, -1.0, -1.0, 1.0, m_TNone, NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	



	////Timer
	pUImanager->CreateUINonNormalRect(0.96, 0.88, -0.055, 0.055, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	for (int i = 0; i < 5; i++) {
		h_TimerBar[i] = pUImanager->CreateUINonNormalRect(0.86, 0.84, (-0.12) + 0.05 * i, (-0.08) + 0.05 * i, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true),
			NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	}

	// 시간 표시 layer 1
	h_TImer1 = BringUINum(pUImanager, pResourceManager, 0.95, 0.89, -0.045, -0.03, 0, 1, GetType());
	h_TImer2 = BringUINum(pUImanager, pResourceManager, 0.95, 0.89, -0.025, -0.01, 0, 1, GetType());

	pUImanager->CreateUINonNormalRect(0.95, 0.89, -0.01, 0.01, pResourceManager->BringTexture("Model/Textures/UITexture/TimeDivide.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	h_TImer3 = BringUINum(pUImanager, pResourceManager, 0.95, 0.89, 0.01, 0.025, 0, 1, GetType());
	h_TImer4 = BringUINum(pUImanager, pResourceManager, 0.95, 0.89, 0.03, 0.045, 0, 1, GetType());


	//Weapon
	m_TrifleGun = pResourceManager->BringTexture("Model/Textures/UITexture/rifle.dds", UI_TEXTURE, true);
	m_TshotGun = pResourceManager->BringTexture("Model/Textures/UITexture/shotGun.dds", UI_TEXTURE, true);
	m_TmachineGun = pResourceManager->BringTexture("Model/Textures/UITexture/machineGun.dds", UI_TEXTURE, true);

	h_weapon=pUImanager->CreateUINonNormalRect(-0.7, -0.8, -0.8, -0.63, m_TrifleGun,NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	

	//Item
	pUImanager->CreateUINonNormalRect(-0.8, -0.9, 0.89, 0.95, pResourceManager->BringTexture("Model/Textures/UITexture/syringe.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	/*pUImanager->CreateUINonNormalRect(-0.8, -0.9, 0.9, 0.95, pResourceManager->BringTexture("Model/Textures/UITexture/eye.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);*/
	
	
	// HP
	pUImanager->CreateUINonNormalRect(-0.67, -0.9, -0.98, -0.83, pResourceManager->BringTexture("Model/Textures/UITexture/HPBackground.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	//HP (number) ***test
	h_HP[0]=BringUINum(pUImanager, pResourceManager, -0.75, -0.81, -0.94, -0.92, 1, 1, GetType());
	h_HP[1]=BringUINum(pUImanager, pResourceManager, -0.75, -0.81, -0.92, -0.90, 0, 1, GetType());
	h_HP[2]=BringUINum(pUImanager, pResourceManager, -0.75, -0.81, -0.90, -0.88, 0, 1, GetType());

	// crash -> Hit
	m_TCrashOk = pResourceManager->BringTexture("Model/Textures/UITexture/crashOk3.dds", UI_TEXTURE, true);
	h_crashOk = pUImanager->CreateUINonNormalRect(0.042, -0.047, -0.03, 0.03, m_TNone, NULL, NULL, 0, TEXTUREUSE, GetType(), true);



	//Target
	m_TtargetRifle= pResourceManager->BringTexture("Model/Textures/UITexture/TargetRifle2.dds", UI_TEXTURE, true);
	m_Ttargetshotgun = pResourceManager->BringTexture("Model/Textures/UITexture/TargetShotgun.dds", UI_TEXTURE, true);
	m_Ttargetmachinegun= pResourceManager->BringTexture("Model/Textures/UITexture/TargetMachinegun.dds", UI_TEXTURE, true);

	// rifle
	h_TargetRifle=pUImanager->CreateUINonNormalRect(0.04, -0.04, -0.025, 0.025, m_TtargetRifle,NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	//pUImanager->CreateUINonNormalRect(0.045, -0.045, -0.03, 0.03, pResourceManager->BringTexture("Model/Textures/UITexture/TargetRifle2.dds", UI_TEXTURE, true),
	//	NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	//shotgun
	h_TargetShotgun=pUImanager->CreateUINonNormalRect(0.05, -0.05, -0.03, 0.03, m_TNone,NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	// machinegun
	h_TargetMachineGun=pUImanager->CreateUINonNormalRect(0.055, -0.055, -0.035, 0.035, m_TNone,NULL, NULL, 0, TEXTUREUSE, GetType(), true);


	//거미 아이콘
	pUImanager->CreateUINonNormalRect(0.95, 0.83, -0.98, -0.92, pResourceManager->BringTexture("Model/Textures/UITexture/SpiderIcon.dds", UI_TEXTURE, true), NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	//Kill Count
	pUImanager->CreateUINonNormalRect(0.96, 0.88, -0.93, -0.8, pResourceManager->BringTexture("Model/Textures/UITexture/KillCount.dds", UI_TEXTURE, true), NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	h_KillCount1= BringUINum(pUImanager, pResourceManager, 0.89, 0.84, -0.92, -0.905, 0, 0, GetType());
	h_KillCount2 = BringUINum(pUImanager, pResourceManager, 0.89, 0.84, -0.9, -0.885, 0, 0, GetType());
	h_KillCount3 = BringUINum(pUImanager, pResourceManager, 0.89, 0.84, -0.88, -0.865, 0, 0, GetType());

	Texture* tempTexture = NULL;
	for (int i = 0; i < 10; ++i) {
		tempTexture = m_pResourceManager->BringTexture(ReturnTexAddress(i).c_str(), UI_TEXTURE, true);
		numTexture.push_back(tempTexture);
	}

	m_pResourceManager->BringTexture("Model/Textures/UITexture/TimerBAR(T).dds", UI_TEXTURE, true);




	
}

void GamePlayScene::ReleaseObjects()
{
	ReleaseShaderVariables();

	if (m_pPlayer) m_pPlayer->Release();

	if (m_pLights) delete[] m_pLights;

	if (m_pGhostTraillerShader) m_pGhostTraillerShader->Release();

	if(m_pBoundigShader) m_pBoundigShader->Release();

	if (m_pPlaneShader) m_pPlaneShader->Release();

	if(m_pDepthShader)m_pDepthShader->Release();
	if (m_pDepthSkinnedShader)m_pDepthSkinnedShader->Release();

	if (m_RedZoneShader) m_RedZoneShader->Release();

	if (m_pNevMeshShader) m_pNevMeshShader->Release();
	if (m_pNevMeshBaker) delete m_pNevMeshBaker;
	if (m_pPathFinder)	delete m_pPathFinder;

	if (m_pQuadTree)
		m_pQuadTree->Release();
	for (auto& GO : m_pGameObject) {
		GO->Release();
	}

	for (int i = 0; i < m_pSceneObject.size(); ++i) {
		if(m_pSceneObject[i])
		m_pSceneObject[i]->Release();
	}
	
	
	if (m_pskybox)m_pskybox->Release();
	if (m_pPerceptionRangeMesh) m_pPerceptionRangeMesh->Release();

	if (m_pTestBox) m_pTestBox->Release();
	if (m_pCollisionManager) delete m_pCollisionManager;
	if (m_pBillObject) m_pBillObject->Release();
	if (m_pRedZoneEffect) m_pRedZoneEffect->Release();

	if (m_BillShader) m_BillShader->Release();

		for (int i = 0; i < 29; ++i) {
			for (auto& B : m_pBloodBillboard[i]) {
				if (B) B->Release();
			}
		}
	

	if (m_pParticleShader) m_pParticleShader->Release();

	for (auto& GO : m_pParticleObjects) {
		if (GO) {
			GO->Release();
		}
	}


	for (auto& GO : m_itemBoxes) {
		if (GO) {
			GO->Release();
		}
	}

	if (m_RedZone) m_RedZone->Release();
	//총알
	for (auto& b : bulletcasings) {
		b->Release();
	}

	if (m_ItemBoxExplosion)m_ItemBoxExplosion->Release(); 

	if (m_pShotgunEffect) m_pShotgunEffect->Release();
}

void GamePlayScene::PlayerControlInput()
{
	static UCHAR pKeysBuffer[256];
	float AddAcel = 20.0f;

	//플레이어 씬일때만 작동하도록 설정하기.
	if (GetKeyboardState(pKeysBuffer)) {
		//애니메이션 상태정의를 위한 플레이어 상태 정의
		DWORD dwDirection = 0;
		DWORD dwPlayerState = STATE_IDLE;

		//Move
		if (pKeysBuffer[W] & 0xF0)
			dwDirection |= DIR_FORWARD;
		
			
		if (m_pPlayer->m_WeaponState == RIGHT_HAND) {
			if (pKeysBuffer[S] & 0xF0)
				dwDirection |= DIR_BACKWARD;
			if (pKeysBuffer[A] & 0xF0)
				dwDirection |= DIR_LEFT;
			if (pKeysBuffer[D] & 0xF0)
				dwDirection |= DIR_RIGHT;
		}
		//하나라도 안눌려있으면 0
		if (!((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT))) {
			m_pPlayer->m_xmf3Velocity.x = 0.0f;
			m_pPlayer->m_xmf3Velocity.z = 0.0f;
				
		}
		//W S A D 키입력 검사
		//RUN
		if ((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT)) {
			if (pKeysBuffer[L_SHIFT] & 0xF0)
			{
				AddAcel += PlayerRunAcel;
				dwPlayerState = STATE_RUN;
						//총기 소유 안할때

					if (m_pPlayer->m_WeaponState == SPINE_BACK) {
						m_pPlayer->SetMaxXZVelocity(7.0f);
						AddAcel += NoGrapAcel;
					}
					else {
						//총기소유하고 걸을때
						m_pPlayer->SetMaxXZVelocity(4.0f);
					}
			
			}
			else {
				//걸을때
				dwPlayerState = STATE_WALK;
				//총기 소유 안할때
				if (m_pPlayer->m_WeaponState == SPINE_BACK) {
					m_pPlayer->SetMaxXZVelocity(5.0f);
					AddAcel += NoGrapAcel;
				}
				else {
					m_pPlayer->SetMaxXZVelocity(3.5f);
				}
			}
		}

		if (pKeysBuffer['1'] & 0xF0) {

			m_pPlayer->ChangeRifle();
		}
		
		if (pKeysBuffer['2'] & 0xF0) {

			m_pPlayer->ChangeShotgun();
		}
		if (pKeysBuffer['3'] & 0xF0) {

			m_pPlayer->Chagnemachinegun();
		}
	
		//JUMP
		if (!m_pPlayer->isJump && pKeysBuffer[SPACE_BAR] & 0xF0) {
			//방향
			dwDirection |= DIR_JUMP_UP;
			// 플레이어 상태
			dwPlayerState |= STATE_JUMP;
			m_pPlayer->isJump = true;
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

		if (m_pPlayer->GetShootingCoolTime() < m_pPlayer->m_ReloadTime && m_pPlayer->m_WeaponState == RIGHT_HAND) {


			static int SignCount = 0;

			if (pKeysBuffer[L_MOUSE] & 0xF0) {
				
				for (auto& b : bulletcasings) {
					if (b->m_bActive == false) {
						b->m_bActive = true;
						b->m_bcrushed = false;
						b->m_xmf4x4ToParent = m_pPlayer->m_xmf4x4ToParent;
						
						XMFLOAT3 pos = XMFLOAT3(
							m_pPlayer->m_LeftHand->m_xmf4x4World._41,
							m_pPlayer->m_LeftHand->m_xmf4x4World._42,
							m_pPlayer->m_LeftHand->m_xmf4x4World._43);
						b->SetPosition(pos);
						b->m_xmf3Velocity = m_pPlayer->GetRight();
						b->m_xmf3Velocity.y = 0;
						break;
					}
				}


				float Sign = 0;
				if (SignCount & 1) {
					Sign = 1;
				}
				else {
					Sign = -1;
				}
				;
				SignCount++;
				float AMP = m_pPlayer->GetAmp();
				m_pPlayer->Rotate(-AMP, Sign* AMP*0.5, 0.0f);
				
			/*	m_pCamera->m_recoiVector.z -= m_pPlayer->GetzRange(false);
				if (m_pPlayer->GetzRange(false) < m_pCamera->m_recoiVector.z)
					m_pCamera->m_recoiVector.z = -m_pPlayer->GetzRange(true);*/



				dwPlayerState |= STATE_SHOOT;
				m_bcrashOk = m_pCollisionManager->CollsionBulletToEnemy(m_pBloodBillboard, m_KillCount);
				m_pCollisionManager->CollisionBulletToItemBox(m_ItemBoxExplosion);
				m_pBillObject->active = true;
				m_pPlayer->m_ReloadTime = 0;
				m_bisCameraShaking = true;
			}
			else {
				m_bisCameraShaking = false;
				m_pCamera->m_bOnceShaking = true;
				m_pCamera->m_recoiVector.x = 0.0f;
				m_pCamera->m_recoiVector.y = 0.0f;
				m_pCamera->m_recoiVector.z = 0.0f;
			}
		}
	/*	else {
			m_pCamera->m_recoiVector.z += 0.1f;
			if (m_pCamera->m_recoiVector.z > 0) m_pCamera->m_recoiVector.z = 0;
		}*/

		//플레이어 애니메이션 적용
		if (m_pPlayer)
			((PlayerAnimationController*)(m_pPlayer->m_pSkinnedAnimationController))->SetAnimationFromInput(dwDirection, dwPlayerState);




		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		static POINT m_ptOldCursorPos = { WINDOWS_POS_X + FRAME_BUFFER_WIDTH / 2 , WINDOWS_POS_Y + FRAME_BUFFER_WIDTH / 2 };

		//SetCursor(NULL);
#if defined(_DEBUG)
		if (pKeysBuffer['X'] & 0xF0) {
		
			m_pPlayer->SetMaxXZVelocity(50.0f);
			AddAcel += 40;
		}
		if (pKeysBuffer[R_MOUSE] & 0xF0) {
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
			if (m_pPlayer->m_WeaponState == RIGHT_HAND) {
				m_bScopeMode = true;
				m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0.15f, 2.4f, 20.0f));
			}
		}
		else {
			m_bScopeMode = false;
			m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0.15f, 2.4f, -1.7f));
		}


		if (pKeysBuffer['P'] & 0xF0) {
			for (const auto& game : m_pGameObject) {
				((AlienSpider*)game)->m_pBrain->SetInactive();
			}

		}

#else

		if (pKeysBuffer['P'] & 0xF0) {
			for (const auto& game : m_pGameObject) {
				((AlienSpider*)game)->m_pBrain->SetInactive();
			}

		}

		if (pKeysBuffer['X'] & 0xF0) {

			m_pPlayer->SetMaxXZVelocity(50.0f);
			AddAcel += 40;
		}
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);

		if (pKeysBuffer[R_MOUSE] & 0xF0 && m_pPlayer->m_WeaponState == RIGHT_HAND) {


			m_bScopeMode = true;
			m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0.35f, 2.4f, 10.0f));
		}
		else {
			m_bScopeMode = false;
			m_pPlayer->GetCamera()->SetOffset(XMFLOAT3(0.35f, 2.4f, -1.7f));
		}

#endif

		if (m_pPlayer)
		m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);


		if (dwDirection) if (m_pPlayer)
			m_pPlayer->CalVelocityFromInput(dwDirection, AddAcel, m_fElapsedTime);
	}

	if (m_pPlayer)
		m_pPlayer->UpdatePosition(m_fElapsedTime);
}

void GamePlayScene::AnimateObjects(float fTimeElapsed)
{

	if (m_bisCameraShaking) {
		//intensity, duration
		m_pCamera->UpdateCameraShake(5.0f, 0.3f, fTimeElapsed,m_pPlayer->m_gunType);
	}

	m_fElapsedTime = fTimeElapsed;
	m_pPlayer->m_ReloadTime += fTimeElapsed;
	PlayerControlInput();
	m_pCollisionManager->CollisionPlayerToStaticObeject();
	m_pCollisionManager->CollisionPlayerToItemBox();
	m_pCollisionManager->CollisionPlayerToEnemy();

	for (auto& GO : m_pGameObject) {

		if (GO->m_bActive) {
			((AlienSpider*)(GO))->Update(fTimeElapsed);

			if(((AlienSpider*)(GO))->m_GoalType !=Jump_Goal)
				((AlienSpider*)(GO))->m_pPerception->IsLookPlayer(m_pPlayer);
			else if(((AlienSpider*)(GO))->m_pSoul->m_JumpStep >= JUMP_LANDING)
				((AlienSpider*)(GO))->m_pPerception->IsLookPlayer(m_pPlayer);
			GO->Animate(fTimeElapsed);
			
		}
	}
	m_pCollisionManager->CollisionEnemyToStaticObeject();
	m_pCollisionManager->CollisionEnemyToPlayer();
	m_bCrashRedZone=m_pCollisionManager->CollisionPlayerToRedZone();





	m_RedZoneHurt += fTimeElapsed;
	if (m_RedZoneHurt > 5.0f) {
		m_RedZoneHurt = 0.0f;
		if (m_bCrashRedZone && m_pPlayer->m_HP > 0) {
			m_pPlayer->m_HP -= 1;
			m_isHurt = true;
		}
	}



	if (m_isHurt) {
		m_hurtAnimation += fTimeElapsed;
		if (m_hurtAnimation > 0.2f) {
			m_isHurt = false;
			m_hurtAnimation = 0.0f;
		}
	}


	m_pPlayer->Animate(fTimeElapsed);
	
	for (auto& GO : m_pBillObjects) {
		if (GO->doAnimate) {
			GO->Animate(fTimeElapsed);
		}
	}

	m_pBillObject->Animate(fTimeElapsed);

	if (m_pRedZoneEffect->doAnimate) {
		m_pRedZoneEffect->Animate(fTimeElapsed);
	}

	if (m_ItemBoxExplosion->doAnimate) {
		m_ItemBoxExplosion->Animate(fTimeElapsed);
	}

	for (int i = 0; i < 29; ++i) {
		for (auto& B : m_pBloodBillboard[i]) {
			if (B->doAnimate) {
				B->Animate(fTimeElapsed);
			}
		}
	}
	//crashUIAnimation
	if (m_bcrashOk) {
		m_crashAnimation += fTimeElapsed;
		if (m_crashAnimation > 0.3f) {
			m_bcrashOk = false;
			m_crashAnimation = 0.0f;
		}
	}
	// 총알 
	for (auto& b : bulletcasings) {
		if (b->m_bActive)
			b->Update(fTimeElapsed);
	}

	m_pCollisionManager->CollisionBulletToObject();


	//절두체 컬링
	m_pCollisionManager->CheckVisiableEnemy();
	m_pPlayer->m_xmf3FinalPosition = m_pPlayer->m_xmf3Position;
}

void GamePlayScene::BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pBoundigShader->OnPrepareRender(pd3dCommandList);
	XMFLOAT3 xmfloat3;
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	for (auto& GO : m_pGameObject) {

		if (GO->m_bActive) {
			if (((AlienSpider*)(GO))->m_pPlayer != NULL) {

				XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
				xmfloat3 = XMFLOAT3(0.0f, 1.0, 0.0f);
				pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
				XMFLOAT3 POS(m_pPlayer->m_xmf4x4World._41, m_pPlayer->m_xmf4x4World._42 + AISIGHTHEIGHT, m_pPlayer->m_xmf4x4World._43);
				xmf4x4World._41 = POS.x;
				xmf4x4World._42 = POS.y;
				xmf4x4World._43 = POS.z;

				XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
				pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
				m_pTestBox->Render(pd3dCommandList, 0);
			}

			xmfloat3 = XMFLOAT3(0.0f, 0.0, 1.0f);
			pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);

			xmf4x4World = GO->m_xmf4x4World;
			xmf4x4World._42 = AISIGHTHEIGHT;
			XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
			pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
			GO->PerceptRangeRender(pd3dCommandList);

			xmfloat3 = XMFLOAT3(1.0f, 0.0, 0.0f);
			pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
			//눈 레이저
			((AlienSpider*)(GO))->PerceptionBindRender(pd3dCommandList);

		}
	}

	if (true) {
		xmf4x4World = Matrix4x4::Identity();
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		//에일리언 경로
		//////파란색
		m_pBoundigShader->OnPrepareRender(pd3dCommandList);
		xmfloat3 = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		for (auto& GO : m_pGameObject) {

			if (GO->m_bActive) {
				((AlienSpider*)(GO))->RouteRender(pd3dCommandList);
			}
		}



		//쿼드 트리
		//초록색
		xmfloat3 = XMFLOAT3(1.0f, 1.0f, 0);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		m_pBoundigShader->OnPrepareRender(pd3dCommandList);
		m_pQuadTree->BoundingRendering(pd3dCommandList, m_DepthRender);


		//////네비메쉬
		m_pNevMeshShader->OnPrepareRender(pd3dCommandList);
		m_pNevMeshBaker->BoundingRendering(pd3dCommandList);


		xmfloat3 = XMFLOAT3(1.0f,0.0f,0.0f);
		XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		m_pBoundigShader->OnPrepareRender(pd3dCommandList);
		m_pCollisionManager->RenderBoundingBox(pd3dCommandList);
		
	}
}

void GamePlayScene::UpdateUI() {	
	int minute, second;
	if (TotalPlayTime >= 10 * 60) {
		//10분경과 -> 게임종료
		h_TImer1->RenderTexture = numTexture[1];
		h_TImer2->RenderTexture = numTexture[0];
		h_TImer3->RenderTexture = numTexture[0];
		h_TImer4->RenderTexture = numTexture[0];
	}
	else {
		minute = TotalPlayTime / 60;
		if (minute >= 10) {
			int ten_s = minute / 10;
			int one_s = minute % 10;
			h_TImer3->RenderTexture = numTexture[ten_s];
			h_TImer4->RenderTexture = numTexture[one_s];
		}
		else {
			h_TImer2->RenderTexture = numTexture[minute];
		}
		
		second = TotalPlayTime % 60;
		if (second >= 10) {
			int ten_s = second / 10;
			int one_s = second % 10;
			h_TImer3->RenderTexture = numTexture[ten_s];
			h_TImer4->RenderTexture = numTexture[one_s];
		}
		else {
			h_TImer3->RenderTexture = numTexture[0];
			h_TImer4->RenderTexture = numTexture[second];
		}
	}

	//타이머 바 표시
	int division = TotalPlayTime / 120;

	for (int i = 1; i < 6; i++) {
		if (division == i) {
			h_TimerBar[i-1]->RenderTexture = m_pResourceManager->BringTexture("Model/Textures/UITexture/TimerBAR(T).dds", UI_TEXTURE, true);
		}
	}

	// Weapon 상태 업데이트
	if (m_pPlayer->m_gunType == HAVE_RIFLE) {
		h_weapon->RenderTexture = m_TrifleGun;

		h_TargetRifle->RenderTexture = m_TtargetRifle;
		h_TargetShotgun->RenderTexture = m_TNone;
		h_TargetMachineGun->RenderTexture = m_TNone;
	}
	else if(m_pPlayer->m_gunType == HAVE_SHOTGUN) {
		h_weapon->RenderTexture = m_TshotGun;

		h_TargetRifle->RenderTexture = m_TNone;
		h_TargetShotgun->RenderTexture = m_Ttargetshotgun;
		h_TargetMachineGun->RenderTexture = m_TNone;
	}
	else if (m_pPlayer->m_gunType == HAVE_MACHINEGUN) {
		h_weapon->RenderTexture = m_TmachineGun;

		h_TargetRifle->RenderTexture = m_TNone;
		h_TargetShotgun->RenderTexture = m_TNone;
		h_TargetMachineGun->RenderTexture = m_Ttargetmachinegun;
	}

	if (m_bScopeMode) {
		if (m_pPlayer->m_gunType == HAVE_SHOTGUN) {
			h_scopeMode->RenderTexture = m_TscopeShoot2;
		}
		else {
			h_scopeMode->RenderTexture = m_TscopeShoot;
		}
		
		h_TargetRifle->RenderTexture = m_TNone;
		h_TargetShotgun->RenderTexture = m_TNone;
		h_TargetMachineGun->RenderTexture = m_TNone;
	}
	else {
		h_scopeMode->RenderTexture = m_TNone;
	}




	if (m_bcrashOk) {
		h_crashOk->RenderTexture = m_TCrashOk;
	}
	else {
		h_crashOk->RenderTexture = m_TNone;
	}

	if (m_pPlayer->m_HP<100) {
		h_HP[0]->RenderTexture = m_TNone;
		h_HP[1]->RenderTexture = numTexture[m_pPlayer->m_HP / 10];
		h_HP[2]->RenderTexture = numTexture[m_pPlayer->m_HP % 10];

	}


	if (m_isHurt) {
		h_HurtState->RenderTexture = m_TbloodScreen;
	}
	else {
		h_HurtState->RenderTexture = m_TNone;
	}
	
	//kill count update
	h_KillCount1->RenderTexture= numTexture[m_KillCount / 100];
	h_KillCount2->RenderTexture = numTexture[m_KillCount / 10];
	h_KillCount3->RenderTexture = numTexture[m_KillCount % 10];

}

void GamePlayScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	float LifeTime = 20.0f;
	TotalPlayTime = static_cast<int>(m_PlayTimeTimer.GetTotalTime());
	m_currentMinute = static_cast<int>(TotalPlayTime / LifeTime);
	//쉐이더로 전체 시간 보내기
	float totaltime = m_PlayTimeTimer.GetTotalTime();
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &totaltime, 41);
	float bredzone = m_bCrashRedZone;
	// 플레이어 방사능에 있는지 여부...
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &bredzone, 40);
	//속도에 따른 블러링
	
	//if (m_pPlayer) {
	//	XMFLOAT3 vel = m_pPlayer->GetVelocity();
	//	float velocity = sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
	//	int	 velo = int(velocity);
	//	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &velo, 39);
	//}
	//

	//카메라 초기화
	if (m_pCamera) {
		m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_pCamera->UpdateShaderVariables(pd3dCommandList);
	}


	UpdateShaderVariables(pd3dCommandList);
	                                                                                                                                                                                     
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (true) {
		m_pskybox->Render(pd3dCommandList, m_pPlayer->GetCamera(), m_pPlayer);
	
		for (auto& GO : m_pGameObject) {
			if (GO->m_bActive) {
				if(GO->m_bVisible) GO->Render(pd3dCommandList);
			}
		}
		m_pPlayer->Render(pd3dCommandList);
		

		for (auto& GO : m_pSceneObject) {
			if (m_pCamera->IsInFrustum(GO->m_BoundingBox)) GO->Render(pd3dCommandList);
		}

	}

	if(m_bBoundingRender) BoudingRendering(pd3dCommandList);

	for (auto& b : bulletcasings) {
		if (b->m_bActive) {
			b->UpdateTransform();
			b->Render(pd3dCommandList);

		}
	}

	for (auto& GO : m_pBillObjects) {
		if (GO->active) {
			GO->Render(pd3dCommandList, m_pPlayer->GetCamera());
		}
	}


	if (m_pBillObject->active) {
		m_pBillObject->Render(pd3dCommandList, m_pPlayer->GetCamera());
	}


	for (int i = 0; i < 29; ++i) {
		for (auto& B : m_pBloodBillboard[i]) {
			if (B->active) {
				B->Render(pd3dCommandList, m_pPlayer->GetCamera());
			}
		}
	}

	for (auto& ParticleObject : m_pParticleObjects) {
		if (ParticleObject->m_bActive) {
			ParticleObject->Render(pd3dCommandList);
		}
	}


	if (m_RedZone) {

		if (m_currentMinute > m_LastMinute) {
			m_pRedZoneEffect->SetPosition(m_RedZone->GetPosition());
			m_pRedZoneEffect->active = true;
			m_RedZone->m_xmf4x4ToParent = Matrix4x4::Identity();
			int index = m_pPathFinder->GetInvalidNode();
			m_RedZone->SetPosition(m_pPathFinder->m_Cell[index].m_BoundingBox.Center.x, 0.f, m_pPathFinder->m_Cell[index].m_BoundingBox.Center.z);
			//int RandomPosition = GetRandomFloatInRange(-200.f, 200.f);
			//m_RedZone->SetPosition(RandomPosition, 0, RandomPosition);
			m_RedZone->m_prexmf4x4ToParent = m_RedZone->m_xmf4x4ToParent;
			m_LastMinute = m_currentMinute;
		}

		if (TotalPlayTime % int(LifeTime) == int(LifeTime-1)) {

			m_RedZone->m_xmf4x4ToParent = m_RedZone->m_prexmf4x4ToParent;
			float size = m_PlayTimeTimer.GetTotalTime()- int(m_PlayTimeTimer.GetTotalTime());
			size = 1.0f - size;
			m_RedZone->SetScale(size, size, size);

		}

		m_RedZone->Render(pd3dCommandList);
	}

	if (m_pRedZoneEffect->active) {
		m_pRedZoneEffect->NoSetPositionRender(pd3dCommandList, m_pPlayer->GetCamera());
	}


	for (auto& GO : m_itemBoxes) {
		if (GO->m_bActive) {
			GO->UpdateTransform(NULL);
			GO->Render(pd3dCommandList);
		}
	}

	if (m_ItemBoxExplosion->active) {
		m_ItemBoxExplosion->Render(pd3dCommandList, m_pPlayer->GetCamera());
	}
}

void GamePlayScene::BuildDepthTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pToLightSpaces = new TOLIGHTSPACES;

	UINT ncbDepthElementBytes;

	ncbDepthElementBytes = ((sizeof(TOLIGHTSPACES) + 255) & ~255); //256의 배수
	m_pd3dcbToLightSpaces = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbDepthElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbToLightSpaces->Map(0, NULL, (void**)&m_pcbMappedToLightSpaces);

	XMFLOAT4X4 xmf4x4ToTexture = { 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f };
	m_xmProjectionToTexture = XMLoadFloat4x4(&xmf4x4ToTexture);
}

void GamePlayScene::BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex)
{
	


	//고정씬
	if (CameraIndex == 0 && !alreadyMaking) {
		PrepareDepthTexture(pCamera, CameraIndex);
		pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		pCamera->UpdateShaderVariables(pd3dCommandList);

		BakeDepthTextureForStatic(pd3dCommandList);
		alreadyMaking = true;
	}
	else if (CameraIndex == 1) {
		PrepareDepthTexture(pCamera, CameraIndex);
		pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		pCamera->UpdateShaderVariables(pd3dCommandList);

		BakeDepthTextureForDynamic(pd3dCommandList);
	}
	else if (CameraIndex == 2) {
		PrepareDepthTexture(pCamera, CameraIndex);
		pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		pCamera->UpdateShaderVariables(pd3dCommandList);
		BakeDepthTextureForStatic(pd3dCommandList);
		BakeDepthTextureForDynamic(pd3dCommandList);

	}
	


}

void GamePlayScene::BakeDepthTextureForStatic(ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_pDepthShader->OnPrepareRender(pd3dCommandList);

	for (auto& GO : m_pSceneObject) {
		GO->DepthRender(pd3dCommandList);
	}

}

void GamePlayScene::BakeDepthTextureForDynamic(ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_pDepthSkinnedShader->OnPrepareRender(pd3dCommandList);

	m_pPlayer->DepthRender(pd3dCommandList);

	for (auto& GO : m_pGameObject) {
		if (GO->m_bActive) 
			if (GO->m_bVisible)
			GO->DepthRender(pd3dCommandList);
	}

}

void GamePlayScene::PrepareDepthTexture(Camera* pCamera , int CameraIndex)
{

		if (m_pLights[CameraIndex].m_bEnable)
		{
			XMFLOAT3 xmf3Position = m_pLights[CameraIndex].m_xmf3Position;
			XMFLOAT3 xmf3Look = m_pLights[CameraIndex].m_xmf3Direction;
			XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, +1.0f, 0.0f);

			XMMATRIX xmmtxLightView = XMMatrixLookToLH(XMLoadFloat3(&xmf3Position), XMLoadFloat3(&xmf3Look), XMLoadFloat3(&xmf3Up));

			float fNearPlaneDistance = 1.0f, fFarPlaneDistance = m_pLights[CameraIndex].m_fRange;

			XMMATRIX xmmtxProjection;
			if (m_pLights[CameraIndex].m_nType == DIRECTIONAL_LIGHT)
			{ 
				float fWidth = _PLANE_WIDTH, fHeight = _PLANE_HEIGHT;
				fNearPlaneDistance = 0.0f, fFarPlaneDistance = 1500.0f;
				xmmtxProjection = XMMatrixOrthographicLH(fWidth, fHeight, fNearPlaneDistance, fFarPlaneDistance);
			}
			else if (m_pLights[CameraIndex].m_nType == SPOT_LIGHT)
			{
				float fFovAngle = 60.0f; // m_pLights->m_pLights[j].m_fPhi = cos(60.0f);
				float fAspectRatio = float(_DEPTH_BUFFER_WIDTH) / float(_DEPTH_BUFFER_HEIGHT);
				xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
			}
			else if (m_pLights[CameraIndex].m_nType == POINT_LIGHT)
			{
				//ShadowMap[6]
			}

			pCamera->SetPosition(xmf3Position);
			XMStoreFloat4x4(&pCamera->m_xmf4x4View, xmmtxLightView);
			XMStoreFloat4x4(&pCamera->m_xmf4x4Projection, xmmtxProjection);

			XMMATRIX xmmtxToTexture = XMMatrixTranspose(xmmtxLightView * xmmtxProjection * m_xmProjectionToTexture);
			XMStoreFloat4x4(&m_pToLightSpaces->m_pToLightSpaces[CameraIndex].m_xmf4x4ToTexture, xmmtxToTexture);

			m_pToLightSpaces->m_pToLightSpaces[CameraIndex].m_xmf4Position = XMFLOAT4(xmf3Position.x, xmf3Position.y, xmf3Position.z, 1.0f);


		}
		else
		{
			m_pToLightSpaces->m_pToLightSpaces[CameraIndex].m_xmf4Position.w = 0.0f;
		}
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

	for (auto& b : bulletcasings) {
		b->ReleaseUploadBuffers();
		break;// 어쳐피 공유 인스턴싱이기 때문에 한번만. 
	}

	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	if (m_pskybox) m_pskybox->ReleaseUploadBuffers();

	if (m_pNevMeshBaker) m_pNevMeshBaker->ReleaseUploadBuffers();

	if (m_pPerceptionRangeMesh) m_pPerceptionRangeMesh->ReleaseUploadBuffers();

	if (m_pTestBox) m_pTestBox->ReleaseUploadBuffers();

	if (m_pCollisionManager) m_pCollisionManager->ReleaseUploadBuffers();

	if (itemBox)itemBox->ReleaseUploadBuffers();

	if (m_RedZone) m_RedZone->ReleaseUploadBuffers();

	for (auto& b : m_itemBoxes) {
		b->ReleaseUploadBuffers();
		break;// 어쳐피 공유 인스턴싱이기 때문에 한번만. 
	}


}

void GamePlayScene::TestCameraRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//카메라 초기화
	if (m_pCamera) {
		m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
		m_pCamera->UpdateShaderVariables(pd3dCommandList);
	}


	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	m_pBoundigShader->OnPrepareRender(pd3dCommandList);
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
	xmf4x4World._41 = pCamera->GetPosition().x;
	xmf4x4World._42 = pCamera->GetPosition().y;
	xmf4x4World._43 = pCamera->GetPosition().z;

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	XMFLOAT3 xmfloat3 = XMFLOAT3(1.0f, 0.0, 0.0f);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
	m_pTestBox->Render(pd3dCommandList, 0);
	

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										WinScene Class				  						       //
/////////////////////////////////////////////////////////////////////////////////////////////////////
WinScene::WinScene()
{
}

WinScene::~WinScene()
{
}

void WinScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager, SoundManager* pSoundManager)
{
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//									 	LoseScene Class				  				  		       //
/////////////////////////////////////////////////////////////////////////////////////////////////////
LoseScene::LoseScene()
{
}

LoseScene::~LoseScene()
{
}

void LoseScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager,SoundManager* pSoundManager)
{
	pUImanager->CreateUINonNormalRect(1.0, -1.0, -1.0, 1.0, pResourceManager->BringTexture("Model/Textures/UITexture/LoseScene.dds", UI_TEXTURE, true), NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	pUImanager->CreateUINonNormalRect(-0.3, -0.38, -0.12, -0.035, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true), NULL, &UIControlHelper::GameStart, 1, TEXTUREUSE, GetType(), true);
	pUImanager->CreateUINonNormalRect(-0.3, -0.38, 0.045, 0.12, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true), NULL, &UIControlHelper::GameQuit, 0, TEXTUREUSE, GetType(), true);

}
