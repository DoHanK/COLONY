#pragma once
#include "stdafx.h"
#include "ColonyCamera.h"
#include "ColonyGameObject.h"
#include "ColonyPlayer.h"
#include "ColonyShader.h"
#include "ResourceManager.h"
#include "UiManager.h"
#include "AlienSpider.h"
#define MAX_LIGHTS						16 

#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT				3

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


	virtual UINT GetType() { return Basic; };




};

class GameLobbyScene :public BasicScene {
public:
	GameLobbyScene() {};
	virtual~GameLobbyScene() {};

	virtual void  BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager);
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
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, ResourceManager* pResourceManager, UIManager* pUImanager);
	void ReleaseObjects();
	
	void PlayerControlInput();
	void AnimateObjects(float fTimeElapsed);
	void BoudingRendering(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);

	virtual void ReleaseUploadBuffers();
	virtual UINT GetType() { return GamePlay; };

protected:

	float								m_fElapsedTime = 0.0f;

	LIGHT*								m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource*						m_pd3dcbLights = NULL;
	LIGHTS*								m_pcbMappedLights = NULL;

	Player*								m_pPlayer;
	ThirdPersonCamera*					m_pCamera;
	int									m_nGameObjects = 0;
	vector<GameObject*>					m_pGameObject;
	vector<GameObject*>					m_pSceneObject;

	GhostTraillerShader*				m_pGhostTraillerShader;
	BoundingShader* m_pBoundigShader;
};