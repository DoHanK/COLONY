#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
///////////////////////////////////////////////////////////////////////////////////////
// 리소스를 업로드하려면, 제일먼저 생성되어야함 GameFrameWork에 제일 먼저 생성되어야함
// LoadModel Info를 관리하며, 프로그램 종료시 리소스매니저가 삭제.
///////////////////////////////////////////////////////////////////////////////////////

class ResourceManager {
public:
	ResourceManager(ID3D12Device* pd3dDevice , ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* m_pRootSignature);
	~ResourceManager();

private:
	ID3D12Device*				m_pDevice;
	ID3D12GraphicsCommandList*	m_pGCommandList;
	ID3D12RootSignature*		m_pRootSignature;



	//메쉬 정보 리스트
	list<pair<string, CLoadedModelInfo*>> ModelnfoList;

	//텍스쳐 리스트
	list<pair<string, Texture*>> TextureList;
public:
	//텍스쳐디스크립터힙
	ID3D12DescriptorHeap* pSrvDescriptorHeap = nullptr;
	//디스크립터의 핸들 주소
	static D3D12_GPU_DESCRIPTOR_HANDLE  hGDescriptor;
	static D3D12_CPU_DESCRIPTOR_HANDLE  hCDescriptor;

	//SRV크기
	static UINT  SRVHeapsize;
	//디스크립터힙 카운팅
	static UINT TextureCounting ;

public:
	//메쉬 디스크립터힙 만들기
	void CreateSrvDescriptorHeap(ID3D12Device* pd3dDevice, int numDesc);
	//파일 경로로부터 파일 이름만 저장
	string ObjectNameFromFile(const char* filename);
	//모델 불러오기
	CLoadedModelInfo* BringModelInfo(const char* filename, const char* FileTextureRoute);
	//텍스쳐 불러오기
	Texture* BringTexture(const char* filename,UINT rootparameter,bool bIsDDSFile);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device* m_pd3Device , Texture* pTexture, UINT nRootParameter , bool bAutoIncrement);
};
