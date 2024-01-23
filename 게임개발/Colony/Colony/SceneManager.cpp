#include "SceneManager.h"
#include "stdafx.h"
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

	//뷰포트 정의를 위한 카메라 삭제
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
	//기존 씬 정리
	BasicScene* pScene = m_SceneStack.top();
	m_SceneStack.pop();
	pScene->ReleaseObjects();
	delete pScene;

	m_pUIManager->m_RenderUIList->clear();
	//다음 씬 로딩 및




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

//애니메이션
void SceneManager::AnimationGameObjects(const float& m_ElapsedTime)
{
	m_SceneStack.top()->AnimateObjects(m_ElapsedTime);

}

//랜더링 장면
void SceneManager::RenderScene(ID3D12GraphicsCommandList* pd3dCommandList)
{

	m_SceneStack.top()->Render(pd3dCommandList);

	if(m_pCamera) m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
}

void SceneManager::SetRootSignature(ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
}

void SceneManager::CreateSceneTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_CLEAR_VALUE d3dClearValue = { DXGI_FORMAT_R8G8B8A8_UNORM, { 1.0f, 1.0f, 1.0f, 1.0f } };

	//Create Memory in Gpu
	for (int i = 0; i < TEXTURE_SCENE_NUM; ++i) {
		m_TextureScene[i] = CreateTexture2DResource(pd3dDevice, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 6, 1, DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON, &d3dClearValue);
	}

	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = 6;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_ShaderSourceDescriptor);

	ID3D12Resource* pShaderResource = m_BringGpuTex;

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = {};
	d3dShaderResourceViewDesc.Format = m_BringGpuTex->GetDesc().Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
	d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
	d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dShaderDescriptorHandle = m_ShaderSourceDescriptor->GetCPUDescriptorHandleForHeapStart();
	pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dShaderDescriptorHandle);

	//디스크립터힙에 대한 설명-> 나는 RTV로 쓰겠다.

	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = 6;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_ShaderRtvDescriptor);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_ShaderRtvDescriptor->GetCPUDescriptorHandleForHeapStart();
	RtvView = new D3D12_CPU_DESCRIPTOR_HANDLE[6];
	for (int i = 0; i < 6; ++i) {
		//랜더타겟 뷰에 대한 설명
		D3D12_RENDER_TARGET_VIEW_DESC d3dRenderTargetViewDesc{};
		d3dRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		d3dRenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		d3dRenderTargetViewDesc.Texture2D.MipSlice = 0;
		d3dRenderTargetViewDesc.Texture2D.PlaneSlice = 0;

		d3dRenderTargetViewDesc.Texture2DArray.FirstArraySlice = i;

		d3dRenderTargetViewDesc.Texture2DArray.ArraySize = 1;

		pd3dDevice->CreateRenderTargetView(m_BringGpuTex, &d3dRenderTargetViewDesc, d3dRtvCPUDescriptorHandle);
		RtvView[i] = d3dRtvCPUDescriptorHandle;
		d3dRtvCPUDescriptorHandle.ptr += m_RenderTargetViewSize;
	}

}
