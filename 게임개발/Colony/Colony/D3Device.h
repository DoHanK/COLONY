#pragma once
class D3Device
{
private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;

    int m_nWndClientWidth;
    int m_nWndClientHeight;

    // DXGI ���丮 �������̽��� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<IDXGIFactory4> m_pdxgiFactory;
    // ���� ü�� �������̽��� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pdxgiswapChain;
    // Direct3D ����̽� �������̽��� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12Device> m_pd3dDevice;


    //msaa ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
    bool m_bMsaa4xEnable = false;
    UINT m_nMsaa4xqualityLevels = 0;


    //����ü���� �ĸ� ������ �����̴�.
    static const UINT m_nSwapChainBuffers = 2;


    // ���� Ÿ�� ���� �迭�� ���� ����Ʈ ������
    std::unique_ptr<Microsoft::WRL::ComPtr<ID3D12Resource>> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
     // ���� Ÿ�� ������ �� �������̽��� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
    //����Ÿ�ٵ�ũ���� ������
    UINT m_nRtvDescriptorIncrementSize;


    // ���� ���ۿ� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
    // ���� ��ũ���� �� �������̽��� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
    //���̵�ũ���� ������
    UINT m_nDsvDescriptorIncrementSize; 


    // ��� ť�� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
    // ��� �Ҵ��ڿ� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
    // ��� ����Ʈ�� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;


    // �潺�� ���� ����Ʈ ������
    Microsoft::WRL::ComPtr <ID3D12Fence> m_pd3dFence;
    HANDLE m_hFenceEvent;
public:
	//����̽� ������
	D3Device();
	//����̽� �Ҹ���
	~D3Device();

    //D3Device ����
    bool CreateDevice(HINSTANCE hInstance, HWND hMainWnd);
    //���̷�Ʈ����̽� �ʱ�ȭ
    void CreateDirect3DDevice();

};


