#include "SceneManager.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//								SceneManager Effectvie Manage Scene								   //
/////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SceneManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager(ResourceManager* pResourceManager, UIManager* pUIManager)
{
	m_pResourceManager = pResourceManager;
	m_pUIManager = pUIManager;
	m_pCamera = new Camera();
}

SceneManager::~SceneManager()
{
	BasicScene* pScene;
	while (!m_SceneStack.empty()) {
		m_SceneStack.top()->ReleaseObjects();
		pScene = m_SceneStack.top();
		m_SceneStack.pop();
		delete pScene;
	}

	//����Ʈ ���Ǹ� ���� ī�޶� ����
	if (m_pCamera) delete m_pCamera;

}

void SceneManager::PushScene(BasicScene* Scene, D3Device* Device,bool bBuild = true)
{
	m_SceneStack.push(Scene);

	if (Scene->GetType()== GamePlay) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}

	//Watchout! UploadBuffer 
	if (bBuild) {
		Device->CommandAllocatorReset();
		Device->CommandListReset();
		m_SceneStack.top()->BuildObjects(Device->GetID3DDevice(), Device->GetCommandList(), m_pd3dGraphicsRootSignature, m_pResourceManager, m_pUIManager);
		Device->CloseCommandAndPushQueue();
		Device->WaitForGpuComplete();

		m_SceneStack.top()->ReleaseUploadBuffers();
		if (m_pResourceManager) m_pResourceManager->ReleaseUploadBuffers();
	}
}

void SceneManager::PopScene()
{
	m_pUIManager->m_RenderUIList->clear();
	BasicScene* pScene = m_SceneStack.top();
	m_SceneStack.pop();
	pScene->ReleaseObjects();
	delete pScene;

	if (m_SceneStack.top()->GetType() == GamePlay) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}
}

void SceneManager::ChangeScene(BasicScene* Scene, D3Device* Device)
{
	Device->CommandAllocatorReset();
	Device->CommandListReset();
	//���� �� ����
	BasicScene* pScene = m_SceneStack.top();
	m_SceneStack.pop();
	pScene->ReleaseObjects();
	delete pScene;

	m_pUIManager->m_RenderUIList->clear();
	//���� �� �ε� ��




	m_SceneStack.push(Scene);
	if (m_SceneStack.top()->GetType() == GamePlay) {
		ShowCursor(false);
	}
	else {
		ShowCursor(true);
	}

	m_SceneStack.top()->BuildObjects(Device->GetID3DDevice(), Device->GetCommandList(), m_pd3dGraphicsRootSignature, m_pResourceManager, m_pUIManager);
	Device->CloseCommandAndPushQueue();
	Device->WaitForGpuComplete();

	m_SceneStack.top()->ReleaseUploadBuffers();
	if (m_pResourceManager) m_pResourceManager->ReleaseUploadBuffers();
	
}

//�ִϸ��̼�
void SceneManager::AnimationGameObjects(const float& m_ElapsedTime)
{
	m_SceneStack.top()->AnimateObjects(m_ElapsedTime);

}

//������ ���
void SceneManager::RenderScene(ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_SceneStack.top()->Render(pd3dCommandList);

	if(m_pCamera) m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
}

void SceneManager::SetRootSignature(ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
}
