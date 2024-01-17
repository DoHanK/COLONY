#pragma once
#include "stdafx.h"
#include "ColonyScene.h"
#include "D3Device.h"
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

	Camera* m_pCamera;
public:
	//æ¿ √ﬂ∞° , æ¿ ªË¡¶ , æ¿ πŸ≤Ÿ±‚
	void PushScene(BasicScene* Scene , D3Device* Device,bool bBuild);
	void PopScene();

	void ChangeScene(BasicScene* Scene , D3Device* Device);

	void AnimationGameObjects(const float& m_ElapsedTime);

	void RenderScene(ID3D12GraphicsCommandList* pd3dCommandList);

};

