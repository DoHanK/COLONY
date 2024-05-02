#include "ColonyScene.h"
#include "ColonyShader.h"
#include "ColonyQuadtree.h"


#define QuadtreeDepth 2



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

void GameLobbyScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager)
{
	//pUImanager->CreateUINonNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true), NULL, NULL, 0,  TEXTUREUSE , GetType());

	UIEffectInfo EffectInfo;
	EffectInfo.ColNum = 6;
	EffectInfo.RowNum = 6;
	EffectInfo.SetTime = 0.1f;
	pUImanager->CreateUISpriteNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true),
	pResourceManager->BringTexture("Model/Textures/Explosion_6x6.dds", UI_MASK_TEXTURE, true), EffectInfo, NULL, 1, (MASKUSE | TEXTUREUSE), GetType(),false);

	// -1 ~ 1 ���̷�
	pUImanager->CreateUISpriteNormalRect(0, FRAME_BUFFER_HEIGHT/2, 0, FRAME_BUFFER_WIDTH/2, pResourceManager->BringTexture("Model/Textures/RobbyTexture/PrimaryTexture.dds", UI_TEXTURE, true),
		pResourceManager->BringTexture("Model/Textures/Explosion_6x6.dds", UI_MASK_TEXTURE, true), EffectInfo, &UIControlHelper::TestFunc, 1, (MASKUSE | TEXTUREUSE), GetType(),false);


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
	
}

bool GamePlayScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_pBillObject->active = true;
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
				if (m_pLights[1].m_bEnable)	m_pLights[1].m_bEnable = false;
				else m_pLights[1].m_bEnable = true;
				break;
		case 'M':
					m_pPlayer->AddPosition(XMFLOAT3(0, 5.0F, 0));
				break;
		case 'N':
			m_pPlayer->AddPosition(XMFLOAT3(0, -5.0F, 0));
			break;
			
				
		default:
			break;
		}
		return(false);
	}
}

void GamePlayScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void GamePlayScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[1].m_xmf3Position.y += 2.f;
	m_pLights[1].m_xmf3Direction = m_pPlayer->GetCamera()->m_xmf3Look;



	m_pLights[2].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[2].m_xmf3Position.y += 3.f;

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

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_fRange = 2000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	//m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(-(_PLANE_WIDTH * 0.5f), _PLANE_HEIGHT, 0.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);

	
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 600.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	//m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.2f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.01f, 0.01f, 0.005f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[1].m_xmf3Position.y += 10.0f;
	m_pLights[1].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(0.01f, 0.01f, 0.01f);
	m_pLights[1].m_fFalloff = 15.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights[2].m_fRange = 200.0f;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_pLights[2].m_xmf3Position = m_pPlayer->GetPosition();
	m_pLights[2].m_xmf3Position.y += 10.0f;
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights[2].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.5f, 0.5f);
	m_pLights[2].m_fFalloff = 20.0f;
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
	nReads = (UINT)::fread(&m_nGameObjects, sizeof(int), 1, pFile); //��Ʈ ���� ������Ʈ �� 

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
				pGameObject->m_BoundingBox.Extents.x;
				pGameObject->m_BoundingBox.Extents.z;
				pGameObject->m_BoundingBox.Transform(pGameObject->m_BoundingBox, DirectX::XMLoadFloat4x4(&pGameObject->m_xmf4x4World));

			}

			
			m_pSceneObject.push_back(pGameObject);
			cur_object += 1;
			
		//}

	}

	::fclose(pFile);
}

void GamePlayScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager)
{
	m_pResourceManager = pResourceManager;

	////ī�޶�
	m_pCamera = new ThirdPersonCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	m_pCamera->GenerateProjectionMatrix(1.01, 1000.f, ASPECT_RATIO, 60.f);
	m_pCamera->RegenerateViewMatrix();

	//���̴� �ε�
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

	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_R32_FLOAT };
	m_pDepthSkinnedShader = new DepthSkinnedRenderingShader();
	m_pDepthSkinnedShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pDepthShader = new DepthRenderingShader();
	m_pDepthShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	LoadSceneObjectsFromFile(pd3dDevice, pd3dCommandList, "Model/MainScene.bin","Model/Textures/scene/", pResourceManager,"Model/MainSceneMeshes/");
	//LoadSceneObjectsFromFile(pd3dDevice, pd3dCommandList, "Model/SpaceStationScene.bin", "Model/Textures/scene/", pResourceManager, "Model/SpaceStationMeshes/");


	m_BillShader = new BillboardShader();
	m_BillShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_BillShader->AddRef();

	m_pPlayer = new Player(pd3dDevice, pd3dCommandList, pResourceManager);
	m_pPlayer->SetCamera(((ThirdPersonCamera*)m_pCamera));
	m_pCamera->SetPlayer(m_pPlayer);

	m_pCollisionManager = new CollisionManager(pd3dDevice, pd3dCommandList);
	m_pCollisionManager->LoadCollisionBoxInfo(pd3dDevice, pd3dCommandList, "boundinginfo.bin");
	m_pCollisionManager->EnrollPlayerIntoCapsule(XMFLOAT3(EPSILON, 0.0, EPSILON), 0.3, 1.3, 0.3, m_pPlayer);
	for (auto& GO : m_pSceneObject) {
		GO->m_BoundingBox;
		if ((strstr(GO->m_pstrFrameName, "SM_Mountain_A") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Mountain_B") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Plant_A") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_Plant_B") == NULL
			&& strstr(GO->m_pstrFrameName, "SM_LongGrass") == NULL)) {
			m_pCollisionManager->EnrollObjectIntoBox(false, GO->m_BoundingBox.Center, GO->m_BoundingBox.Extents, GO);

		}
	}

	//Octree Crate
	XMFLOAT3 OctreeScale = m_pScenePlane->m_BoundingBox.Extents;
	
	OctreeScale.y = 20.f;
	XMFLOAT3 OctreeCenter = XMFLOAT3(-1.0f,0,0);

	OctreeCenter.y = 20.f;
	m_pQuadTree = new QuadTree(pd3dDevice, pd3dCommandList, 0, OctreeCenter, OctreeScale);
	m_pQuadTree->BuildTreeByDepth(pd3dDevice, pd3dCommandList, QuadtreeDepth);

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
	Texture* spiderColor[6];
	int i = 0;
	spiderColor[i++] = pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Bone_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_DarkGreen_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Green_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_purple_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i++] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_Red_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);
	spiderColor[i] =pResourceManager->BringTexture("Model/AlienspiderColor/1_Alien_Spider_White_AlbedoTransparency.dds", ALBEDO_TEXTURE, true);

	m_pGameObject.reserve(400);
	for (int j = 0; j < 30; ++j) {
		for (int i = 0; i < 1; i++) {
			int idex;
			do {
				idex = rand() % 90000;
				

			} while (m_pPathFinder->ValidNode(idex));

			AlienSpider* p = new AlienSpider(pd3dDevice, pd3dCommandList, pResourceManager, m_pPathFinder);
			//p->SetPosition(m_pPathFinder->m_Cell[idex].m_BoundingBox.Center.x, 0.f, m_pPathFinder->m_Cell[idex].m_BoundingBox.Center.z);
			p->SetPosition(j*2, 0.f, 0.f);
			p->SetPerceptionRangeMesh(m_pPerceptionRangeMesh);
			p->m_pSkinnedAnimationController->SetTrackAnimationSet(0, (Range_2+j) % AlienAnimationName::EndAnimation);
			p->SetGhostShader(m_pGhostTraillerShader);
			p->m_pSpiderTex = spiderColor[j % 6];
			m_pGameObject.push_back(p);
			m_pCollisionManager->EnrollEnemy(p);
		}
	}
	
	m_pTestBox = new ShphereMesh(pd3dDevice, pd3dCommandList,20,20, PlayerRange);
	

	//billboard test
	m_pBillObject = new Billboard(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		pResourceManager->BringTexture("Model/Textures/shootEffect.dds", BILLBOARD_TEXTURE, true), m_BillShader,m_pPlayer->m_SelectWeapon.FindFrame("Export"));
	m_pBillObject->doAnimate = true;
	m_pBillObject->SetAddPosition(XMFLOAT3(0.0f, 0.3f,0.0f));
	m_pBillObject->SetRowNCol(7, 5);
	m_pBillObject->m_BillMesh->UpdataVertexPosition(UIRect(0.1, -0.1, -0.1, 0.1), 1.0f);
	m_pBillObject->m_BillMesh->UpdateUvCoord(UIRect(1, 0, 0, 1));
	m_pBillObject->SettedTimer = 0.01f;
	m_pBillObject->doOnce = true;
	m_pBillObject->AddRef();

	//billboard -> doAnimate,active,ownerObject,TickAddPosition ����







	BulidUI(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pResourceManager, pUImanager);
	BuildDefaultLightsAndMaterials();
	BuildDepthTexture(pd3dDevice, pd3dCommandList);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void GamePlayScene::BulidUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager)
{////UI

	////Timer

	pUImanager->CreateUINonNormalRect(0.95, 0.85, -0.09, 0.09, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	for (int i = 0; i < 5; i++) {
		h_TimerBar[i] = pUImanager->CreateUINonNormalRect(0.82, 0.8, (-0.17) + 0.07 * i, (-0.11) + 0.07 * i, pResourceManager->BringTexture("Model/Textures/UITexture/TimerBackground.dds", UI_TEXTURE, true),
			NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	}

	// �ð� ǥ�� layer 1
	h_TImer1 = BringUINum(pUImanager, pResourceManager, 0.94, 0.86, -0.08, -0.05, 0, 1, GetType());
	h_TImer2 = BringUINum(pUImanager, pResourceManager, 0.94, 0.86, -0.04, -0.01, 0, 1, GetType());

	pUImanager->CreateUINonNormalRect(0.93, 0.87, -0.01, 0.01, pResourceManager->BringTexture("Model/Textures/UITexture/TimeDivide.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);

	h_TImer3 = BringUINum(pUImanager, pResourceManager, 0.94, 0.86, 0.01, 0.04, 0, 1, GetType());
	h_TImer4 = BringUINum(pUImanager, pResourceManager, 0.94, 0.86, 0.05, 0.08, 0, 1, GetType());




	//Weapon
	pUImanager->CreateUINonNormalRect(-0.7, -0.8, -0.75, -0.55, pResourceManager->BringTexture("Model/Textures/UITexture/Gun.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	//Item

	//HP
	pUImanager->CreateUINonNormalRect(-0.7, -0.9, -0.955, -0.785, pResourceManager->BringTexture("Model/Textures/UITexture/HPBackground.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);
	//HP (number) ***test
	BringUINum(pUImanager, pResourceManager, -0.77, -0.83, -0.9, -0.88, 1, 1, GetType());
	BringUINum(pUImanager, pResourceManager, -0.77, -0.83, -0.88, -0.86, 0, 1, GetType());
	BringUINum(pUImanager, pResourceManager, -0.77, -0.83, -0.86, -0.84, 0, 1, GetType());

	//Target
	pUImanager->CreateUINonNormalRect(0.02, -0.02, -0.015, 0.015, pResourceManager->BringTexture("Model/Textures/UITexture/Target_01_a.dds", UI_TEXTURE, true),
		NULL, NULL, 0, TEXTUREUSE, GetType(), true);




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

	if (m_BillShader) m_BillShader->Release();
}

void GamePlayScene::PlayerControlInput()
{
	static UCHAR pKeysBuffer[256];
	float AddAcel = 20.0f;

	//�÷��̾� ���϶��� �۵��ϵ��� �����ϱ�.
	if (GetKeyboardState(pKeysBuffer)) {
		//�ִϸ��̼� �������Ǹ� ���� �÷��̾� ���� ����
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
		//�ϳ��� �ȴ��������� 0
		if (!((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT))) {
			m_pPlayer->m_xmf3Velocity.x = 0.0f;
			m_pPlayer->m_xmf3Velocity.z = 0.0f;
				
		}
		//W S A D Ű�Է� �˻�
		//RUN
		if ((dwDirection & DIR_FORWARD) || (dwDirection & DIR_BACKWARD) || (dwDirection & DIR_LEFT) || (dwDirection & DIR_RIGHT)) {
			if (pKeysBuffer[L_SHIFT] & 0xF0)
			{
				AddAcel += PlayerRunAcel;
				dwPlayerState = STATE_RUN;
						//�ѱ� ���� ���Ҷ�

					if (m_pPlayer->m_WeaponState == SPINE_BACK) {
						m_pPlayer->SetMaxXZVelocity(7.0f);
						AddAcel += NoGrapAcel;
					}
					else {
						//�ѱ�����ϰ� ������
						m_pPlayer->SetMaxXZVelocity(4.0f);
					}
			
			}
			else {
				//������
				dwPlayerState = STATE_WALK;
				//�ѱ� ���� ���Ҷ�
				if (m_pPlayer->m_WeaponState == SPINE_BACK) {
					m_pPlayer->SetMaxXZVelocity(5.0f);
					AddAcel += NoGrapAcel;
				}
				else {
					m_pPlayer->SetMaxXZVelocity(3.5f);
				}
			}
		}

		
	
		//JUMP
		if (!m_pPlayer->isJump && pKeysBuffer[SPACE_BAR] & 0xF0) {
			//����
			dwDirection |= DIR_JUMP_UP;
			// �÷��̾� ����
			dwPlayerState |= STATE_JUMP;
			m_pPlayer->isJump = true;
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
		}


		if (pKeysBuffer['P'] & 0xF0) {
			for (const auto& game : m_pGameObject) {
				((AlienSpider*)game)->m_pBrain->SetInactive();
			}

		}
#else
		GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 80.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 80.0f;
		SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
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


	m_fElapsedTime = fTimeElapsed;

	PlayerControlInput();


	for (auto& GO : m_pGameObject) {

		((AlienSpider*)(GO))->Update(fTimeElapsed);

		((AlienSpider*)(GO))->m_pPerception->IsLookPlayer(m_pPlayer);
		GO->Animate(fTimeElapsed);

	}
	m_pCollisionManager->CollisionPlayerToStaticObeject();

	m_pPlayer->Animate(fTimeElapsed);
	
	for (auto& GO : m_pBillObjects) {
		if (GO->doAnimate) {
			GO->Animate(fTimeElapsed);
		}
	}

		m_pBillObject->Animate(fTimeElapsed);
	
}

void GamePlayScene::BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pBoundigShader->OnPrepareRender(pd3dCommandList);
	XMFLOAT3 xmfloat3;
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);


	
	for (auto& GO : m_pGameObject) {

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
			m_pTestBox->Render(pd3dCommandList,0);
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
		//�� ������
		((AlienSpider*)(GO))->PerceptionBindRender(pd3dCommandList);

		 xmfloat3=XMFLOAT3(1.0f, 0.0, 0.0f);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		GO->BoudingBoxRender(pd3dCommandList, true);
		
	}

	if (true) {
		xmf4x4World = Matrix4x4::Identity();
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		//���ϸ��� ���
		//////�Ķ���
		m_pBoundigShader->OnPrepareRender(pd3dCommandList);
		xmfloat3 = XMFLOAT3(0.0f, 0.0f, 1.0f);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		for (auto& GO : m_pGameObject) {


			((AlienSpider*)(GO))->RouteRender(pd3dCommandList);
		}


		//m_pPlayer->BoudingBoxRender(pd3dCommandList, true);



		//���� Ʈ��
		//�ʷϻ�
		xmfloat3 = XMFLOAT3(1.0f, 1.0f, 0);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		m_pBoundigShader->OnPrepareRender(pd3dCommandList);
		m_pQuadTree->BoundingRendering(pd3dCommandList, m_DepthRender);


		//////�׺�޽�
		m_pNevMeshShader->OnPrepareRender(pd3dCommandList);
		m_pNevMeshBaker->BoundingRendering(pd3dCommandList);


		XMFLOAT3 xmfloat3 = XMFLOAT3(1, 0, 0);
		XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
		m_pCollisionManager->RenderBoundingBox(pd3dCommandList);
	}
}

void GamePlayScene::UpdateUI() {	
	int TotalPlayTime = static_cast<int>(m_PlayTimeTimer.GetTotalTime());
	int minute, second;
	if (TotalPlayTime >= 10 * 60) {
		//10�а�� -> ��������
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

	//Ÿ�̸� �� ǥ��
	int division = TotalPlayTime / 120;

	for (int i = 1; i < 6; i++) {
		if (division == i) {
			h_TimerBar[i-1]->RenderTexture = m_pResourceManager->BringTexture("Model/Textures/UITexture/TimerBAR(T).dds", UI_TEXTURE, true);
		}
	}
}

void GamePlayScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{

	//�ӵ��� ���� ����
	//if(m_pPlayer.)
	//XMFLOAT3 vel = m_pPlayer->GetVelocity();
	//float velocity = sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
	//int	 velo = int(velocity);
	//	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &velo, 39);


	//ī�޶� �ʱ�ȭ
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
			GO->Render(pd3dCommandList);
		}

		m_pPlayer->Render(pd3dCommandList);

		for (auto& GO : m_pSceneObject) {
			GO->Render(pd3dCommandList);

		}
	}

	if(m_bBoundingRender) BoudingRendering(pd3dCommandList);


	for (auto& GO : m_pBillObjects) {
		if (GO->active) {
			GO->Render(pd3dCommandList, m_pPlayer->GetCamera());
		}
	}



	if (m_pBillObject->active) {
		m_pBillObject->Render(pd3dCommandList, m_pPlayer->GetCamera());
		//test = false;
	}
	
}

void GamePlayScene::BuildDepthTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pToLightSpaces = new TOLIGHTSPACES;

	UINT ncbDepthElementBytes;

	ncbDepthElementBytes = ((sizeof(TOLIGHTSPACES) + 255) & ~255); //256�� ���
	m_pd3dcbToLightSpaces = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbDepthElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbToLightSpaces->Map(0, NULL, (void**)&m_pcbMappedToLightSpaces);

	XMFLOAT4X4 xmf4x4ToTexture = { 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f };
	m_xmProjectionToTexture = XMLoadFloat4x4(&xmf4x4ToTexture);
}

void GamePlayScene::BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex)
{
	PrepareDepthTexture(pCamera, CameraIndex);


	
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	m_pDepthSkinnedShader->OnPrepareRender(pd3dCommandList);

	m_pPlayer->DepthRender(pd3dCommandList);

	for (auto& GO : m_pGameObject) {
		GO->DepthRender(pd3dCommandList);
	}

	m_pDepthShader->OnPrepareRender(pd3dCommandList);

	for (auto& GO : m_pSceneObject) {
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
	//Camera Player...���.

	for (int i = 0; i <1; i++) {
		m_pGameObject[i]->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_pSceneObject.size(); ++i) {
		m_pSceneObject[i]->ReleaseUploadBuffers();
	}

	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	if (m_pskybox) m_pskybox->ReleaseUploadBuffers();

	if (m_pNevMeshBaker) m_pNevMeshBaker->ReleaseUploadBuffers();

	if (m_pPerceptionRangeMesh) m_pPerceptionRangeMesh->ReleaseUploadBuffers();

	if (m_pTestBox) m_pTestBox->ReleaseUploadBuffers();

	if (m_pCollisionManager) m_pCollisionManager->ReleaseUploadBuffers();
}

void GamePlayScene::TestCameraRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//ī�޶� �ʱ�ȭ
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

