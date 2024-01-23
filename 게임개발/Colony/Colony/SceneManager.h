#pragma once
#include "stdafx.h"
#include "ColonyScene.h"
#include "D3Device.h"

#define TEXTURE_SCENE_NUM	6
/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SceneManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class SceneManager
{
public:
	SceneManager(ResourceManager* pResourceManager, UIManager* pUIManager);
	~SceneManager();
	
	std::stack<BasicScene*>	m_SceneStack;
	UINT m_Gamestate;

	//Manager
	ResourceManager*	 m_pResourceManager;
	UIManager*			 m_pUIManager;
	Camera* m_pCamera =NULL;

	//
	ID3D12Resource* m_TextureScene[TEXTURE_SCENE_NUM];
	ID3D12DescriptorHeap* m_ShaderSourceDescriptor;
	ID3D12DescriptorHeap* m_ShaderRtvDescriptor;
	UINT  m_RenderTargetViewSize;
	D3D12_CPU_DESCRIPTOR_HANDLE* RtvView[TEXTURE_SCENE_NUM];
	//루트시그너쳐
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;
public:
	//씬 추가 , 씬 삭제 , 씬 바꾸기
	void PushScene(BasicScene* Scene , D3Device* Device,bool bBuild);
	void PopScene();

	void ChangeScene(BasicScene* Scene , D3Device* Device);

	void AnimationGameObjects(const float& m_ElapsedTime);

	void RenderScene(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetRootSignature(ID3D12RootSignature* pd3dGraphicsRootSignature);

	void CreateSceneTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

};

