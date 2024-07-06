#pragma once
#include "stdafx.h"
#include "ColonyScene.h"
#include "ResourceManager.h"
#include "D3Device.h"

class BasicScene;
class UIManager;
struct UIInfo;
#define TEXTURE_SCENE_NUM	6

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SceneManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class SceneManager
{
public:
	SceneManager(D3Device* pDevice , ResourceManager* pResourceManager, UIManager* pUIManager);
	~SceneManager();
	
	std::stack<BasicScene*>	m_SceneStack;
	UINT m_Gamestate;

	//Manager
	ResourceManager*	 m_pResourceManager = NULL;
	UIManager*			 m_pUIManager = NULL;
	Camera* m_pCamera =NULL;
	//
	D3Device* m_pD3Device = NULL;
	// 씬 텍스쳐
	Texture* m_TextureScene[TEXTURE_SCENE_NUM];

	ID3D12DescriptorHeap* m_ShaderRtvDescriptor = NULL;
	UINT  m_RenderTargetViewSize = 0;
	UINT  m_ShaderResourceViewSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE* RtvView = NULL;
	//루트시그너쳐
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//깊이 텍스쳐
	Texture*		 m_pDepthFromLightTexture[MAX_DEPTH_TEXTURES];
	ID3D12DescriptorHeap* m_SourceDepthTextureDescriptor = NULL;
	ID3D12DescriptorHeap* m_RTDepthTextureDescriptor = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE*			m_RtvDepthView = NULL;

	ID3D12Resource*			m_pd3dDepthBuffer = NULL;
	ID3D12DescriptorHeap* m_DepthDescriptorForDepthTexture = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;
	//텍스쳐
	Camera* m_ppDepthRenderCameras[MAX_DEPTH_TEXTURES];

	UIInfo* p;
public:
	//씬 추가 , 씬 삭제 , 씬 바꾸기
	void PushScene(BasicScene* Scene ,bool bBuild);
	void PopScene();

	void ChangeScene(BasicScene* Scene );

	void AnimationGameObjects(const float& m_ElapsedTime);

	void RenderScene(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetRootSignature(ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateSceneTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void CreateDepthTextureFromLight(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	void RenderDepthScene(ID3D12GraphicsCommandList* pd3dCommandList);


};

