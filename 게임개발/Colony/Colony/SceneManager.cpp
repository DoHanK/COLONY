#include "SceneManager.h"
#include "stdafx.h"
#include "ResourceManager.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//								SceneManager Effectvie Manage Scene								   //
/////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SceneManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
SceneManager::SceneManager(D3Device* pDevice,ResourceManager* pResourceManager, UIManager* pUIManager)
{
	m_pResourceManager = pResourceManager;
	m_pUIManager = pUIManager;
	m_pD3Device = pDevice;
	m_pCamera = new Camera();
	for (int i = 0; i < TEXTURE_SCENE_NUM; ++i) {
		m_TextureScene[i] = new Texture();
	}
	CreateSceneTexture(pDevice->GetID3DDevice(), pDevice->GetCommandList());
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
	
	//텍스쳐 삭제
	for (int i = 0; i < TEXTURE_SCENE_NUM; ++i) {
		m_TextureScene[i]->Release();

	}

	if(m_ShaderRtvDescriptor) m_ShaderRtvDescriptor->Release();
	//뷰포트 정의를 위한 카메라 삭제
	if (m_pCamera) delete m_pCamera;

}

void SceneManager::PushScene(BasicScene* Scene,bool bBuild = true)
{
	m_SceneStack.push(Scene);

	if (Scene->GetType()== GamePlay) {
		while (ShowCursor(false) >= 0);
	}
	else {
		while (ShowCursor(true) < 0);
	}


	//Watchout! UploadBuffer 
	if (bBuild) {
		m_pD3Device->CommandAllocatorReset();
		m_pD3Device->CommandListReset();
		m_SceneStack.top()->BuildObjects(m_pD3Device->GetID3DDevice(), m_pD3Device->GetCommandList(), m_pd3dGraphicsRootSignature, m_pResourceManager, m_pUIManager);
		m_pD3Device->CloseCommandAndPushQueue();
		m_pD3Device->WaitForGpuComplete();

		m_SceneStack.top()->ReleaseUploadBuffers();
		if (m_pResourceManager) m_pResourceManager->ReleaseUploadBuffers();
	}
		m_pUIManager->CreateUINonNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, m_TextureScene[m_SceneStack.top()->GetType()], NULL, NULL, 0, TEXTUREUSE, m_SceneStack.top()->GetType());
}

void SceneManager::PopScene()
{
	for (int i = 0; i < TEXTURE_LAYER; ++i) {
		m_pUIManager->m_RenderUIList[i].clear();
	}

	BasicScene* pScene = m_SceneStack.top();
	m_SceneStack.pop();
	pScene->ReleaseObjects();
	delete pScene;

	if (m_SceneStack.top()->GetType() == GamePlay) {
		while (ShowCursor(false) >= 0);
	}
	else {
		while (ShowCursor(true) < 0);
	}


}

void SceneManager::ChangeScene(BasicScene* Scene)
{
	m_pD3Device->CommandAllocatorReset();
	m_pD3Device->CommandListReset();
	//기존 씬 정리
	BasicScene* pScene = m_SceneStack.top();
	m_SceneStack.pop();
	pScene->ReleaseObjects();
	delete pScene;
	
	for (int i = 0; i < TEXTURE_LAYER; ++i) {
		m_pUIManager->m_RenderUIList[i].clear();
	}
	//다음 씬 로딩 및




	m_SceneStack.push(Scene);

	if (Scene->GetType() == GamePlay) {
		while (ShowCursor(false) >= 0);
	}
	else {
		while (ShowCursor(true) < 0);
	}

	m_SceneStack.top()->BuildObjects(m_pD3Device->GetID3DDevice(), m_pD3Device->GetCommandList(), m_pd3dGraphicsRootSignature, m_pResourceManager, m_pUIManager);
	m_pD3Device->CloseCommandAndPushQueue();
	m_pD3Device->WaitForGpuComplete();

	m_SceneStack.top()->ReleaseUploadBuffers();
	if (m_pResourceManager) m_pResourceManager->ReleaseUploadBuffers();
	
	m_pUIManager->CreateUINonNormalRect(0, FRAME_BUFFER_HEIGHT, 0, FRAME_BUFFER_WIDTH, m_TextureScene[m_SceneStack.top()->GetType()], NULL, NULL, 0, TEXTUREUSE, m_SceneStack.top()->GetType());
}

//애니메이션
void SceneManager::AnimationGameObjects(const float& m_ElapsedTime)
{
	if(!m_SceneStack.empty()) m_SceneStack.top()->AnimateObjects(m_ElapsedTime);

}

//랜더링 장면
void SceneManager::RenderScene(ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_pD3Device->ChangeResourceBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET, m_TextureScene[m_SceneStack.top()->GetType()]->GetTexture(0));
	m_pD3Device->SetRtIntoTexture(m_TextureScene[m_SceneStack.top()->GetType()]->GetTexture(0), RtvView[m_SceneStack.top()->GetType()]);
	if(!m_SceneStack.empty()) m_SceneStack.top()->Render(pd3dCommandList);
	if(m_pCamera) m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pUIManager->AllLayerDrawRect(pd3dCommandList);
	m_pD3Device->ChangeResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON,  m_TextureScene[m_SceneStack.top()->GetType()]->GetTexture(0));


}

void SceneManager::SetRootSignature(ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
}

void SceneManager::CreateSceneTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.125f , 0.3f , 1.0f } };
	//Create Memory in Gpu
	for (int i = 0; i < TEXTURE_SCENE_NUM; ++i) {
		m_TextureScene[i]->SetTexture(0,CreateTexture2DResource(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 6, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue));
		ResourceManager::CreateShaderResourceViews(pd3dDevice, m_TextureScene[i], UI_TEXTURE, FALSE);
	}

	//디스크립터힙에 대한 설명-> 나는 RTV로 쓰겠다.
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = TEXTURE_SCENE_NUM;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_ShaderRtvDescriptor);


	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_ShaderRtvDescriptor->GetCPUDescriptorHandleForHeapStart();
	m_RenderTargetViewSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	RtvView = new D3D12_CPU_DESCRIPTOR_HANDLE[TEXTURE_SCENE_NUM];
	for (int i = 0; i < TEXTURE_SCENE_NUM; ++i) {
		//랜더타겟 뷰에 대한 설명
		D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc{};
		d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
		d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;
		pd3dDevice->CreateRenderTargetView(m_TextureScene[i]->GetTexture(0), &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		RtvView[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += m_RenderTargetViewSize;
	}

}
