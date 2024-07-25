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
#include "CollisionManager.h"
#include "SoundManager.h"
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
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager ,UIManager* pUImanager,SoundManager* pSoundManager) {};
	virtual void ReleaseObjects() {};
	virtual void ReleaseUploadBuffers() {};
	virtual void AnimateObjects(float fTimeElapsed) {};
	virtual void PostRender(int nPipelineState) {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL) {};

	virtual void BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex) {};
	virtual UINT GetType() { return Basic; };

	virtual void UpdateUI() {};


	//Multithread

	virtual void RenderWithMultiThread(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12GraphicsCommandList* pd3dSubCommandList[], int ableThread, Camera* pCamera = NULL) {};
	virtual void AnimateObjectsWithMultithread(float fTimeElapsed) {};

	SoundManager* m_pSoundManager = NULL;
};

class GameLobbyScene :public BasicScene {
public:
	GameLobbyScene() { };
	virtual~GameLobbyScene() {};

	virtual void  BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager,SoundManager* pSoundManager);
	virtual UINT GetType() { return GameLobby; };
	

	Texture* m_TbuttonF; // 커서가 버튼 위에 없을 때
	Texture* m_TbuttonT; // 커서가 버튼 위에 있을 때 

	UIInfo* h_gameButton;
	UIInfo* h_settingButton;
	UIInfo* h_quitButton;
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
	void LoadSceneObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName, const char* TexFileName,ResourceManager* pResourceManager,char* modelLocation);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager,SoundManager* pSoundManager);
	void BulidUI(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager);
	void ReleaseObjects();
	
	void PlayerControlInput();
	void AnimateObjects(float fTimeElapsed);
	void BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);

	virtual void BuildDepthTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BakeDepthTexture(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int CameraIndex);
	virtual void BakeDepthTextureForStatic(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BakeDepthTextureForDynamic(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void PrepareDepthTexture(Camera* pCamera, int CameraIndex);
	virtual void ReleaseUploadBuffers();
	virtual UINT GetType() { return GamePlay; };

	virtual void TestCameraRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	virtual void UpdateUI();

	float GetRandomFloatInRange(float minVal, float maxVal);
	
	//Multithread
	void AnimateObjectsWithMultithread(float fTimeElapsed);
	virtual void RenderWithMultiThread(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12GraphicsCommandList* pd3dSubCommandList[], int ableThread, Camera* pCamera = NULL);

	XMFLOAT3 GetMainScene() { return XMFLOAT3(56.160427, 0.099998, -4.593836); }

	XMFLOAT3 GetSubScene() { return XMFLOAT3(72.728699 ,9.658748 , -3.842552); }

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
	//쉐이더
	PlaneShader*						m_pPlaneShader = NULL;
	NevMeshShader*						m_pNevMeshShader = NULL;
	GhostTraillerShader*				m_pGhostTraillerShader =NULL;
	BoundingShader*						m_pBoundigShader = NULL;
	//그림자 쉐이더
	DepthSkinnedRenderingShader*		m_pDepthSkinnedShader = NULL;
	DepthRenderingShader*				m_pDepthShader = NULL;
	BillboardShader*					m_BillShader=NULL;
	bool								alreadyMaking = false;

	bool								m_bBoundingRender = false;
	int									m_DepthRender = 0;


	PerceptionRangeMesh*				m_pPerceptionRangeMesh = NULL;
	ShphereMesh*						m_pTestBox;

	ColonyTimer							m_PlayTimeTimer;


	ResourceManager*					m_pResourceManager;


	UIInfo*								h_TImer1;
	UIInfo*								h_TImer2;
	UIInfo*								h_TImer3;
	UIInfo*								h_TImer4;
	UIInfo*								h_weapon;
	UIInfo*								h_TimerBar[5];
	UIInfo*								h_HP[3];
	UIInfo*								h_TargetRifle;
	UIInfo*								h_TargetShotgun;
	UIInfo*								h_TargetMachineGun;
	UIInfo*								h_HurtState;
	UIInfo*								h_KillCount1;
	UIInfo*								h_KillCount2;
	UIInfo*								h_KillCount3;
	

	vector<Texture*>					numTexture;
	Texture*							m_TrifleGun;
	Texture*							m_TmachineGun;
	Texture*							m_TshotGun;
	Texture* m_TtargetRifle;
	Texture* m_Ttargetshotgun;
	Texture* m_Ttargetmachinegun;
	Texture* m_TscopeShoot;
	Texture* m_TscopeShoot2;
	Texture* m_TbloodScreen;

	CollisionManager*					m_pCollisionManager;

	// Billboard
	Billboard*							m_pBillObject;
	vector <Billboard*>					m_pBillObjects;
	vector<Billboard*>					m_pBloodBillboard[29];
	bool								test = false;
	Billboard*							m_pRedZoneEffect;
	Billboard*							m_pShotgunEffect;
	vector<Billboard*>					m_InFoBillBoard;
	vector<Billboard*>					m_DeadEneyEffect[MAX_THREAD_NUM];


	// Particle
	ParticleShader*						m_pParticleShader=NULL;
	vector< ParticleObject*>			m_pParticleObjects;
	

	//Item
	UINT m_nItemBox = 0; 
	vector<GameObject*>					m_itemBoxes;
	vector<Item*>						m_items;
	vector<Billboard*>					m_ItemBoxExplosion;


	GameObject* itemBox = NULL;


	vector<BulletCasing*> bulletcasings;

	//crashUIAnimation
	UIInfo* h_crashOk;
	Texture* m_TCrashOk=NULL;
	Texture* m_TNone = NULL;
	bool	m_bcrashOk = false;
	float m_crashAnimation = 0.0f;

	//scope mode
	bool m_bScopeMode = false;
	UIInfo* h_scopeMode;


	//Red zone
	RedZoneShader* m_RedZoneShader = NULL;
	RedZone* m_RedZone = NULL;
	int m_LastMinute = 0;
	int m_currentMinute = 0;
	bool m_bCrashRedZone = false;
	float m_RedZoneHurt = 0.0f;

	//total time
	int TotalPlayTime = 0;

	//hurtUI
	bool m_isHurt = false;
	float m_hurtAnimation = 0.0f;

	bool m_bisCameraShaking = false;

	//적 죽인수
	int m_KillCount = 0;

	//Sound
	LPDIRECTSOUNDBUFFER SpaceShipBGM = nullptr;
	LPDIRECTSOUNDBUFFER PlaySceneBGM = nullptr;
	LPDIRECTSOUNDBUFFER RainBGM = nullptr;
	LPDIRECTSOUNDBUFFER StepSound = nullptr;
	LPDIRECTSOUNDBUFFER JumpSound = nullptr;
	LPDIRECTSOUNDBUFFER RifleSound = nullptr;
	LPDIRECTSOUNDBUFFER ShotgunSound = nullptr;
	LPDIRECTSOUNDBUFFER MachineGunSound = nullptr;
	LPDIRECTSOUNDBUFFER SpiderHurt = nullptr;
	LPDIRECTSOUNDBUFFER ReloadSound = nullptr;
	LPDIRECTSOUNDBUFFER ScopeModeSound = nullptr;
	LPDIRECTSOUNDBUFFER AllReloadSound = nullptr;
	LPDIRECTSOUNDBUFFER OkeySound = nullptr;
	LPDIRECTSOUNDBUFFER ElectronicSound = nullptr;
	LPDIRECTSOUNDBUFFER alarmSound = nullptr;
	LPDIRECTSOUNDBUFFER BigExplosion = nullptr;
	LPDIRECTSOUNDBUFFER PickUpSound = nullptr;

	LPDIRECTSOUNDBUFFER PickItem = nullptr;


	bool ScopeSoundOnce = true;
	bool StepSoundOnce = true;
	bool OkeySoundOnce = true;


	/// SubScene Object 
	GameObject* m_pSceneSpaceShip = NULL;
	GameObject* m_pSpaceShipMap = NULL;

	//INFO BillBorad


	UIInfo*								m_InFoUI;

	


	int							m_Progress = 0;
	enum SceneProgress {
		GoMoniterNCheckMission,
		GoOutSide
	};
	enum BillInfoNum {
		mission,


	};

	// MultiThread 
	std::vector<QuadTree*> m_Quadlist;

	int quadranderingidx = 0;


public:
	enum OP_TYPE { ANIMATION, RENDERING, END };

	std::vector<std::thread> m_threads;

	Concurrency::concurrent_queue<pair<int, long long >> m_Joblist[MAX_THREAD_NUM];  ///first -> op , second -> extra info

	void ThreadWorker(int threadnum);
	volatile int readycount = 0;



	

};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										WinScene Class				  						       //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class WinScene :public BasicScene {
public:
	WinScene();
	virtual ~WinScene();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager, SoundManager* pSoundManager);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//									 	LoseScene Class				  				  		       //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class LoseScene :public BasicScene {
public:
	LoseScene();
	virtual ~LoseScene();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* m_pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager, SoundManager* pSoundManager);
};