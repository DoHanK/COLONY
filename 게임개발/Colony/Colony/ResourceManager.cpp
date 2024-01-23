#include "ResourceManager.h"
#include "stdafx.h"
#include "ColonyGameObject.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//					 ResourceManager Manage Reource that is Texture and Mesh Info				   //
/////////////////////////////////////////////////////////////////////////////////////////////////////



 D3D12_GPU_DESCRIPTOR_HANDLE  ResourceManager::hGDescriptor ;
 D3D12_CPU_DESCRIPTOR_HANDLE  ResourceManager::hCDescriptor;
 UINT ResourceManager::TextureCounting;
 UINT ResourceManager::SRVHeapsize;

 D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
 {
	 D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	 d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	 d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	 switch (nTextureType)
	 {
	 case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	 case RESOURCE_TEXTURE2D_ARRAY:
		 d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		 d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		 d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		 d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		 d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		 break;
	 case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		 d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		 d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		 d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		 d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		 d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		 d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		 d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		 break;
	 case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		 d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		 d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		 d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		 d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		 break;
	 case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		 d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		 d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		 d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		 d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		 d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		 break;
	 }
	 return(d3dShaderResourceViewDesc);
 }


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										ResourceManager Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
ResourceManager::ResourceManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* m_pRootSignature)
{
	//텍스쳐디스크립터힙 생성
	CreateSrvDescriptorHeap(pd3dDevice, 100);
	hGDescriptor = pSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	hCDescriptor = pSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	SRVHeapsize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	TextureCounting = 0;

	m_pDevice = pd3dDevice;
	m_pRootSignature = m_pRootSignature;
	m_pGCommandList = pCommandList;
	
}

ResourceManager::~ResourceManager()
{
	//모델 삭제
	for (pair<string, CLoadedModelInfo*>& Model : ModelnfoList) {
		//애니메이션데이터도 삭제
		for (int i = 0; i < Model.second->m_nSkinnedMeshes; i++)
		{
			Model.second->m_ppAnimationSets[i]->Release();
		}
		
		Model.second->m_pModelRootObject->Release();

		delete Model.second;
	}

	for (pair<string, Texture*>& TextureInfo : TextureList) {
		TextureInfo.second->Release();
	}


	if (pSrvDescriptorHeap) {
		pSrvDescriptorHeap->Release();
		pSrvDescriptorHeap = nullptr;
	}

}

void ResourceManager::CreateSrvDescriptorHeap(ID3D12Device* pd3dDevice, int numDesc)
{
	if (pSrvDescriptorHeap) {
		pSrvDescriptorHeap->Release();
		pSrvDescriptorHeap = nullptr;
	}

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = numDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&pSrvDescriptorHeap));

}

string ResourceManager::ObjectNameFromFile(const char* filename)
{
	string MeshName = filename;
	string MeshTag = "";


	//뒤에서 부터 단어를 fatch할거기 때문에 역반복자를 이용함.
	auto pName = std::find(MeshName.rbegin(), MeshName.rend(), '/').base();

	for (; pName != MeshName.end(); pName++)
	{
		if (*pName == '.')
			break;

		MeshTag += *pName;
	}

	return MeshTag;
}

CLoadedModelInfo* ResourceManager::BringModelInfo(const char* filename,const char* FileTextureRoute)
{

	string CheckTag = ObjectNameFromFile(filename);

	auto pCheck = find_if(ModelnfoList.begin(), ModelnfoList.end(), [&CheckTag](pair<string, CLoadedModelInfo*> element) {return element.first == CheckTag; });

	//메쉬정보를 발견 했을때
	if (pCheck != ModelnfoList.end()) {
		//메쉬 정보를 반환한다.
		return	(*pCheck).second;
	}
	//발견 못함.
	else {

		//파일을 읽어 들이고 정보를 받아옴.
		CLoadedModelInfo* LoadObjectModelInfo = GameObject::LoadGeometryAndAnimationFromFile(m_pDevice, m_pGCommandList, NULL, filename, NULL, FileTextureRoute);
		ModelnfoList.push_back({ CheckTag,LoadObjectModelInfo });
		LoadObjectModelInfo->m_pModelRootObject->AddRef();

		//스키니모델이 있으면
		for (int i = 0; i < LoadObjectModelInfo->m_nSkinnedMeshes; i++)
		{
	
			LoadObjectModelInfo->m_ppAnimationSets[i]->AddRef();

		}
		

		return LoadObjectModelInfo;
	}


	return nullptr;
}

void ResourceManager::ReleaseUploadBuffers()
{
	for (auto& Texture : TextureList) {
		Texture.second->ReleaseUploadBuffers();
	}
}

Texture* ResourceManager::BringTexture(const char* filename, UINT RootParameter, bool bIsDDSFile)
{

	string TargetName = ObjectNameFromFile(filename);

	auto  pPosition = find_if(TextureList.begin(), TextureList.end(), [TargetName](pair<string, Texture*> element) {return element.first == TargetName; });

	if (TextureList.end() == pPosition) {
	
		Texture* TempTexture = new Texture(1, RESOURCE_TEXTURE2D, 0);
	
		// 필요한 버퍼 크기 계산
		size_t bufferSize = mbstowcs(NULL, filename, 0);
		// 변환된 와이드 문자열을 저장할 버퍼 할당
		wchar_t* wideStringFile = new wchar_t[bufferSize + 1];
		// 멀티바이트에서 와이드 문자열로 변환
		mbstowcs(wideStringFile, filename, bufferSize + 1);

		TempTexture->LoadTextureFromFile(m_pDevice, m_pGCommandList, wideStringFile, 0 , bIsDDSFile);
		//정보가 없을때
		ResourceManager::CreateShaderResourceViews(m_pDevice,TempTexture, RootParameter, false);
		TextureList.push_back({ TargetName, TempTexture });
		TempTexture->AddRef();

		return TempTexture;
	}
	else {

		//해당 타겟 리소스 반환
		return pPosition->second;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceManager::CreateShaderResourceViews(ID3D12Device* m_pd3Device, Texture* pTexture, UINT nRootParameter, bool bAutoIncrement)
{																	//리소스 정보, 몇번째 루트 파라메타인지, 루트시그너쳐자동으로 증가여부(False)
	

	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = hGDescriptor;
	d3dSrvGPUDescriptorHandle.ptr += TextureCounting * SRVHeapsize;

	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		int nTextureType = pTexture->GetTextureType();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetTexture(i);
			D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
			m_pd3Device->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, hCDescriptor);

			pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameter + i) : nRootParameter, d3dSrvGPUDescriptorHandle);
		}

		hCDescriptor.ptr += SRVHeapsize;
		TextureCounting++;
	}
	return(d3dSrvGPUDescriptorHandle);
}
