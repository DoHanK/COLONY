#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
///////////////////////////////////////////////////////////////////////////////////////
// ���ҽ��� ���ε��Ϸ���, ���ϸ��� �����Ǿ���� GameFrameWork�� ���� ���� �����Ǿ����
// LoadModel Info�� �����ϸ�, ���α׷� ����� ���ҽ��Ŵ����� ����.
///////////////////////////////////////////////////////////////////////////////////////

class ResourceManager {
public:
	ResourceManager(ID3D12Device* pd3dDevice , ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* m_pRootSignature);
	~ResourceManager();

private:
	ID3D12Device*				m_pDevice;
	ID3D12GraphicsCommandList*	m_pGCommandList;
	ID3D12RootSignature*		m_pRootSignature;



	//�޽� ���� ����Ʈ
	list<pair<string, CLoadedModelInfo*>> ModelnfoList;

	//�ؽ��� ����Ʈ
	list<pair<string, Texture*>> TextureList;
public:
	//�ؽ��ĵ�ũ������
	ID3D12DescriptorHeap* pSrvDescriptorHeap = nullptr;
	//��ũ������ �ڵ� �ּ�
	static D3D12_GPU_DESCRIPTOR_HANDLE  hGDescriptor;
	static D3D12_CPU_DESCRIPTOR_HANDLE  hCDescriptor;

	//SRVũ��
	static UINT  SRVHeapsize;
	//��ũ������ ī����
	static UINT TextureCounting ;

public:
	//�޽� ��ũ������ �����
	void CreateSrvDescriptorHeap(ID3D12Device* pd3dDevice, int numDesc);
	//���� ��ηκ��� ���� �̸��� ����
	string ObjectNameFromFile(const char* filename);
	//�� �ҷ�����
	CLoadedModelInfo* BringModelInfo(const char* filename, const char* FileTextureRoute);
	//�ؽ��� �ҷ�����
	Texture* BringTexture(const char* filename,UINT rootparameter,bool bIsDDSFile);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViews(ID3D12Device* m_pd3Device , Texture* pTexture, UINT nRootParameter , bool bAutoIncrement);
};
