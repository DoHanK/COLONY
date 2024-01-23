#pragma once
#include "stdafx.h"
#include "ColonyScene.h"
#include "D3Device.h"


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
	ID3D12Resource* m_BringGpuTex;
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
};

