#pragma once
class D3Device
{
private:

	HINSTANCE					m_hInstance;
	HWND						m_hWnd;

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;

	IDXGIFactory4* m_pdxgiFactory = NULL;
	IDXGISwapChain3* m_pdxgiSwapChain = NULL;
	ID3D12Device* m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;
	UINT m_nMsaa4xqualityLevels = 0;
	ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;

	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;

	ID3D12CommandAllocator* m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue* m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList* m_pd3dCommandList = NULL;

	ID3D12Fence* m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;

	UINT m_nCbvSrvDescriptorIncrementSize;
	UINT m_nRtvDescriptorIncrementSize;
	UINT m_nDsvDescriptorIncrementSize;

	D3D12_RESOURCE_BARRIER m_d3dResourceBarrier;
public:
	//디바이스 생성자
	D3Device();
	//디바이스 소멸자
	~D3Device();

	//D3Device 생성
	bool CreateDevice(HINSTANCE hInstance, HWND hMainWnd);

	//D3Devcie 파괴
	void DestroyDevice();
	//다이렉트디바이스 초기화
	void CreateDirect3DDevice();
	//명령큐와 리스트
	void CreateCommandQueueAndList();
	//스왑 체인
	void CreateSwapChain();
	//랜더타겟 뷰
	void CreateRenderTargetViews();
	//깊이 스텐실 뷰
	void CreateDepthStencilView();
	//깊이, 랜더타겟 디스크립터 힙 만들기
	void CreateRtvAndDsvDescriptorHeaps();

	void ChangeSwapChainState();

	void WaitForGpuComplete();

	void MoveToNextFrame();

	//commandList
	void CommandListReset();
	void CommandAllocatorReset() { m_pd3dCommandAllocator->Reset(); };
	void CloseCommandAndPushQueue();
	ID3D12GraphicsCommandList* GetCommandList() { return m_pd3dCommandList; }
	ID3D12Device* GetID3DDevice() { return m_pd3dDevice; }

	void MakeResourceBarrier();
	void CloseResourceBarrier();
	void RtAndDepthReset();

	void PresentScreen() { m_pdxgiSwapChain->Present(0, 0); }

};


