#pragma once
class D3Device
{
private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;

    int m_nWndClientWidth;
    int m_nWndClientHeight;

    // DXGI 팩토리 인터페이스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_pdxgiFactory;
    // 스왑 체인 인터페이스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pdxgiswapChain;
    // Direct3D 디바이스 인터페이스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12Device> m_pd3dDevice;


    //msaa 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
    bool m_bMsaa4xEnable = false;
    UINT m_nMsaa4xqualityLevels = 0;


    //스왑체인의 후면 버퍼의 개수이다.
    static const UINT m_nSwapChainBuffers = 2;


    // 렌더 타겟 버퍼 배열에 대한 스마트 포인터
    std::unique_ptr<Microsoft::WRL::ComPtr<ID3D12Resource>> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
     // 렌더 타겟 서술자 힙 인터페이스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
    //렌더타겟디스크립터 사이즈
    UINT m_nRtvDescriptorIncrementSize;


    // 깊이 버퍼에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
    // 깊이 디스크립터 힙 인터페이스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
    //깊이디스크립터 사이즈
    UINT m_nDsvDescriptorIncrementSize; 


    // 명령 큐에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
    // 명령 할당자에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
    // 명령 리스트에 대한 스마트 포인터
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;


    // 펜스에 대한 스마트 포인터
    Microsoft::WRL::ComPtr <ID3D12Fence> m_pd3dFence;
    HANDLE m_hFenceEvent;
public:
	//디바이스 생성자
	D3Device();
	//디바이스 소멸자
	~D3Device();

    //D3Device 생성
    bool CreateDevice(HINSTANCE hInstance, HWND hMainWnd);
    //다이렉트디바이스 초기화
    void CreateDirect3DDevice();

};


