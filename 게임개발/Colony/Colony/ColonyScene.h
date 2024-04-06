#pragma once
#include "stdafx.h"
#include "ColonyCamera.h"
#include "ColonyGameObject.h"
#include "ColonyPlayer.h"
#include "ColonyShader.h"
#include "ResourceManager.h"
#include "UiManager.h"
#include "AlienSpider.h"
#include "ColonyQuadtree.h"
#include "NevMeshBaker.h"
#include "UIControlHelper.h"
#include "PathFinder.h"
#include "PerceptionMesh.h"
#include "ColonyTimer.h"

class UIManager;
struct UIInfo;
class ResourceManager;

struct LIGHT
{
	XMFLOAT4							m_xmf4Ambient;
	XMFLOAT4							m_xmf4Diffuse;
	XMFLOAT4							m_xmf4Specular;
	XMFLOAT3							m_xmf3Position;
	float 								m_fFalloff;
	XMFLOAT3							m_xmf3Direction;
	float 								m_fTheta; //cos(m_fTheta)
	XMFLOAT3							m_xmf3Attenuation;
	float								m_fPhi; //cos(m_fPhi)
	bool								m_bEnable;
	int									m_nType;
	float								m_fRange;
	float								padding;
};

struct LIGHTS
{
	LIGHT								m_pLights[MAX_LIGHTS];
	XMFLOAT4							m_xmf4GlobalAmbient;
	int									m_nLights;
};

struct TOOBJECTSPACEINFO
{
	XMFLOAT4X4						m_xmf4x4ToTexture;
	XMFLOAT4						m_xmf4Position;
};

struct TOLIGHTSPACES
{
	TOOBJECTSPACEINFO				m_pToLightSpaces[MAX_LIGHTS];
};

enum SceneType{
	Basic,
	GameLobby,
	GamePlay
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										BasicScene Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BasicScene
{
public:
	BasicScene() {};
	virtual ~BasicScene() {};

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; };
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; };

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void ReleaseShaderVariables() {};

	virtual void BuildDefaultLightsAndMaterials() {};

	//necessary Function
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager ,UIManager* pUImanager) {};
	virtual void ReleaseObjects() {};
	virtual void ReleaseUploadBuffers() {};
	virtual void AnimateObjects(float fTimeElapsed) {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL) {};

	virtual void BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex) {};
	virtual UINT GetType() { return Basic; };

	virtual void UpdateUI() {};


};

class GameLobbyScene :public BasicScene {
public:
	GameLobbyScene() {};
	virtual~GameLobbyScene() {};

	virtual void  BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager);
	virtual UINT GetType() { return GameLobby; };
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										GamePlayScene Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class GamePlayScene :public BasicScene {
public:
	GamePlayScene();
	virtual ~GamePlayScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void LoadSceneObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName, const char* TexFileName,ResourceManager* pResourceManager);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager);
	void ReleaseObjects();
	
	void PlayerControlInput();
	void AnimateObjects(float fTimeElapsed);
	void BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);

	virtual void BuildDepthTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex);
	virtual void PrepareDepthTexture(Camera* pCamera, int CameraIndex);
	virtual void ReleaseUploadBuffers();
	virtual UINT GetType() { return GamePlay; };

	virtual void TestCameraRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual void UpdateUI();

protected:

	float								m_fElapsedTime = 0.0f;

	LIGHT*								m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource*						m_pd3dcbLights = NULL;
	LIGHTS*								m_pcbMappedLights = NULL;
protected:
	TOLIGHTSPACES*						m_pToLightSpaces;
	ID3D12Resource*						m_pd3dcbToLightSpaces = NULL;
	TOLIGHTSPACES*						m_pcbMappedToLightSpaces = NULL;
	XMMATRIX							m_xmProjectionToTexture;

protected:

	Player*								m_pPlayer = NULL;
	ThirdPersonCamera*					m_pCamera = NULL;

	int									m_nGameObjects = 0;
	SkyBox*								m_pskybox = NULL;
	GameObject*							m_pScenePlane = NULL;
	vector<GameObject*>					m_pGameObject;
	vector<GameObject*>					m_pSceneObject;


	QuadTree*							m_pQuadTree = NULL;
	PathFinder*							m_pPathFinder = NULL;
	NevMeshBaker*						m_pNevMeshBaker = NULL;
	//Ω¶¿Ã¥ı
	PlaneShader*						m_pPlaneShader = NULL;
	NevMeshShader*						m_pNevMeshShader = NULL;
	GhostTraillerShader*				m_pGhostTraillerShader =NULL;
	BoundingShader*						m_pBoundigShader = NULL;
	//±◊∏≤¿· Ω¶¿Ã¥ı
	DepthSkinnedRenderingShader*		m_pDepthSkinnedShader = NULL;
	DepthRenderingShader*				m_pDepthShader = NULL;


	bool								m_bBoundingRender = false;
	int									m_DepthRender = 0;

	PerceptionRangeMesh*					m_pPerceptionRangeMesh = NULL;
	ShphereMesh*							m_pTestBox;

	ColonyTimer							m_PlayTimeTimer;


	ResourceManager*					m_pResourceManager;


	UIInfo*								h_TImer1;
	UIInfo*								h_TImer2;
	UIInfo*								h_TImer3;
	UIInfo*								h_TImer4;
	
	UIInfo* h_TimerBar[5];
};