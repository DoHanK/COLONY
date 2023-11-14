#include "stdafx.h"
#include "D3Device.h"

D3Device::D3Device()
{
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
}

D3Device::~D3Device()
{

}

bool D3Device::CreateDevice(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;





	return true;
}

void D3Device::CreateDirect3DDevice()
{

}

