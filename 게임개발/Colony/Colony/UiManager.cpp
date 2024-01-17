#include "UiManager.h"
#include "stdafx.h"
#include "ColonyMesh.h"


UIManager::UIManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pMesh.reserve(TEXTRUE_MASH* TEXTURE_LAYER);
	m_Shader = new UIShader();
	m_Shader->AddRef();
	m_Shader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	for (int i = 0; i < TEXTRUE_MASH * TEXTURE_LAYER; i++) {

		m_pMesh.push_back(new UIRectMesh(pd3dDevice, pd3dCommandList));
		m_pMesh[m_pMesh.size() - 1]->AddRef();
	}
}

UIManager::~UIManager()
{
	for (int i = 0; i < TEXTRUE_MASH * TEXTURE_LAYER; i++) {

		m_pMesh[i]->Release();
	}
	if (m_Shader)
		m_Shader->Release();

	m_RenderUIList->clear();
}

UIRect UIManager::CreateNormalizePixel(float top, float bottom, float left, float right)
{
	//y coord
	float NormalTop = 1 - ((top * 2) / FRAME_BUFFER_HEIGHT);
	float NormalBottom = 1 - ((bottom * 2) / FRAME_BUFFER_HEIGHT);
	//x coord
	float NormalLeft = -1 + ((left * 2) / FRAME_BUFFER_WIDTH);
	float NormalRight = -1 + ((right * 2) / FRAME_BUFFER_WIDTH);

	return { NormalTop, NormalBottom, NormalLeft, NormalRight };
}

bool UIManager::CreateUINonNormalRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, bool(*f)(void* argu),int Layer)
{
	// is not invalid 
	if (!(top < bottom && right > left))
		return false;

	UIRect* PRect = new UIRect{ CreateNormalizePixel(top, bottom, left, right) };
	//Init RootSignature for UIShader
	if(tex) tex->SetRootSignatureIndex(0, UI_TEXTURE);
	if(Masktex) Masktex->SetRootSignatureIndex(0, UI_MASK_TEXTURE);


	m_RenderUIList[Layer].push_back({UIInfo{PRect,tex,Masktex}, f});


	return true;
}

void UIManager::AllLayerDrawRect(ID3D12GraphicsCommandList* pd3d12CommandList)
{

	int count = 0;
	if (m_pMesh.size() != 0) {

		m_Shader->OnPrepareRender(pd3d12CommandList);
		//Rander Order 0 -> 1 -> 2
		for (int i = 0 ; i < TEXTURE_LAYER; ++i) {
			
			for (const pair<UIInfo, bool(*)(void*)>& info : m_RenderUIList[i]) {

			
				if (info.first.RenderTexture) {
					info.first.RenderTexture->UpdateShaderVariable(pd3d12CommandList,0);
				}
				if (info.first.MaskTexture) {
					m_pMesh[count]->UpdateMaskValue(1.0f, 1.0f, 1.0f, 1.0f);
					info.first.MaskTexture->UpdateShaderVariable(pd3d12CommandList, 0);
				}
				else {
					m_pMesh[count]->UpdateMaskValue(0.0f, 0.0f, 0.0f, 0.0f);
				}

				m_pMesh[count]->UpdateVertexPosition((*info.first.Rect));
				m_pMesh[count++]->Render(pd3d12CommandList);
			}
		}

	}

}


