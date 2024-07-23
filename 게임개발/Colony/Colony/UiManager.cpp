#include "UiManager.h"
#include "stdafx.h"
#include "ColonyMesh.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//								UIManager give 3Layer Ui and Sprite UI 							   //
/////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										UIManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

UIManager::UIManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pUIMesh.reserve(TEXTRUE_MASH* TEXTURE_LAYER);
	m_pUISceneMesh.reserve(TEXTURE_SCENE_NUM);
	m_Shader = new UIShader();
	m_Shader->AddRef();
	m_Shader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	for (int i = 0; i < TEXTRUE_MASH * TEXTURE_LAYER; i++) {

		m_pUIMesh.push_back(new UIRectMesh(pd3dDevice, pd3dCommandList));
		m_pUIMesh[i]->AddRef();
	}

	for (int i = 0; i < TEXTURE_SCENE_NUM; i++) {

		m_pUISceneMesh.push_back(new UIRectMesh(pd3dDevice, pd3dCommandList));
		m_pUISceneMesh[i]->AddRef();
	}
}

UIManager::~UIManager()
{



	if (m_Shader)
		m_Shader->Release();


	for (int i = 0; i < TEXTRUE_MASH * TEXTURE_LAYER; i++) {

		m_pUIMesh[i]->Release();
	}

	for (int i = 0; i < TEXTURE_LAYER; i++) {

		for (auto& UI : m_RenderUIList[i]) {
			if (UI.first.EffectInfo) delete UI.first.EffectInfo;

		}
	}



	for (auto& UI : m_pRenderTargetList) {
		if (UI.first.EffectInfo) delete UI.first.EffectInfo;

	}

	for (int i = 0; i < TEXTURE_SCENE_NUM; i++) {

		m_pUISceneMesh[i]->Release();
	}


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

// UISystem
UIInfo* UIManager::CreateUINonNormalRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, std::function<void(UIControlHelper&)> f,int Layer,UINT option ,UINT SceneType, bool bNormal)
{
	// is not invalid
	if(!bNormal)
	if (!(top < bottom && right > left))
		return false;


	UIRect PRect;
	if (bNormal) {
		PRect = UIRect{ top, bottom, left, right };

	}
	else {
		PRect = UIRect{ CreateNormalizePixel(top, bottom, left, right) };

	}
	//Init RootSignature for UIShader
	if(tex) tex->SetRootSignatureIndex(0, UI_TEXTURE);
	if(Masktex) Masktex->SetRootSignatureIndex(0, UI_MASK_TEXTURE);

	//Nosprite
	m_RenderUIList[Layer].push_back({UIInfo{SceneType,PRect,option,NULL,tex,Masktex}, f});


	return &m_RenderUIList[Layer].back().first;
}

UIInfo* UIManager::CreateUISpriteNormalRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, UIEffectInfo Uieffect, std::function<void(UIControlHelper&)> f, int Layer, UINT option, UINT SceneType, bool bNormal)
{
	// is not invalid 
	if (!bNormal)
	if (!(top < bottom && right > left))
		return false;
	UIRect PRect;
	if (bNormal) {
		PRect = UIRect{ UIRect (top, bottom, left, right)};

	}
	else {
		PRect = UIRect{ CreateNormalizePixel(top, bottom, left, right) };

	}
	UIEffectInfo* pEffectInfo = new UIEffectInfo(Uieffect);

	//Init RootSignature for UIShader
	if (tex) tex->SetRootSignatureIndex(0, UI_TEXTURE);
	if (Masktex) Masktex->SetRootSignatureIndex(0, UI_MASK_TEXTURE);

	//Nosprite
	m_RenderUIList[Layer].push_back({ UIInfo{SceneType,PRect,option,pEffectInfo,tex,Masktex}, f });

	return &m_RenderUIList[Layer].back().first;
}

UIInfo* UIManager::CreateRederTargetRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, std::function<void(UIControlHelper&)> f, int Layer, UINT option, UINT SceneType, bool bNormal) {

	// is not invalid 
	if (!bNormal)
	if (!(top < bottom && right > left))
		return false;

	UIRect PRect;
	if (bNormal) {
		PRect = UIRect{ top, bottom, left, right };

	}
	else {
		PRect = UIRect{ CreateNormalizePixel(top, bottom, left, right) };

	}
	//Init RootSignature for UIShader
	if (tex) tex->SetRootSignatureIndex(0, UI_TEXTURE);
	if (Masktex) Masktex->SetRootSignatureIndex(0, UI_MASK_TEXTURE);

	//Nosprite
	m_pRenderTargetList.push_front({ UIInfo{SceneType,PRect,option,NULL,tex,Masktex}, f });


	return &m_pRenderTargetList.back().first;

}

void UIManager::AnimateUI(float ElapsedTime)
{
	for (int i = 0; i < TEXTURE_LAYER; ++i) {

		for (pair<UIInfo, std::function<void(UIControlHelper&)>>& info : m_RenderUIList[i]) {

			if (info.first.EffectInfo) {
		
				info.first.EffectInfo->Timer += ElapsedTime;

				if (info.first.EffectInfo->Timer > info.first.EffectInfo->SetTime) {
					info.first.EffectInfo->Timer = 0;
				}

			if (info.first.EffectInfo->Timer == 0) {
				info.first.EffectInfo->NowCol++;

				if (info.first.EffectInfo->NowCol == info.first.EffectInfo->ColNum) {
					info.first.EffectInfo->NowRow++;
					info.first.EffectInfo->NowCol = 0;
				}

				if (info.first.EffectInfo->NowRow == info.first.EffectInfo->RowNum) {
					info.first.EffectInfo->NowRow = 0;
					info.first.EffectInfo->NowCol = 0;
				}
			}
			}

		
		}
	}

}

void UIManager::AllLayerDrawRect(ID3D12GraphicsCommandList* pd3d12CommandList, int SceneType)
{

	int count = 0;
	if (m_pUIMesh.size() != 0) {

		m_Shader->OnPrepareRender(pd3d12CommandList);
		//Rander Order 0 -> 1 -> 2
		for (int i = 0 ; i < TEXTURE_LAYER; ++i) {
			
			for (const pair<UIInfo, std::function<void(UIControlHelper&)>>& info : m_RenderUIList[i]) {

				if (info.first.SceneType == SceneType) {

					//텍스쳐 있는지 없는지 확인
					if (info.first.RenderTexture) {
						if(pd3d12CommandList)
							info.first.RenderTexture->UpdateShaderVariable(pd3d12CommandList, 0);

					}
					if (info.first.MaskTexture) {

						info.first.MaskTexture->UpdateShaderVariable(pd3d12CommandList, 0);
					}

					//옵션
					m_pUIMesh[count]->UpdateMaskValue(info.first.Option);

					//SPRITEANIMATION에 따른 UV 설정
					if (info.first.EffectInfo) {
						float m_row = info.first.EffectInfo->NowRow;
						float m_rows = info.first.EffectInfo->RowNum;
						float m_col = info.first.EffectInfo->NowCol;
						float m_cols = info.first.EffectInfo->ColNum;;
						m_pUIMesh[count]->UpdateMaskUvCoord(UIRect{ m_row * (1.0f / m_rows) , (m_row + 1.0f) * (1.0f / m_rows) , m_col * (1.0f / m_cols), (m_col + 1.f) * (1.0f / m_cols) });
					}
					else {
						m_pUIMesh[count]->UpdateMaskUvCoord(UIRect(0.0f, 1.0f, 0.0f, 1.0f));
					}

					//랜더링 텍스쳐 설정
					m_pUIMesh[count]->UpdateVertexPosition((info.first.Rect));
					m_pUIMesh[count++]->Render(pd3d12CommandList);
				}
			}
		}

	}

}
//DrawRenderScene
void UIManager::DrawScene(ID3D12GraphicsCommandList* pd3d12CommandList,int SceneType)
{
	int count = 0;
	m_Shader->OnPrepareRender(pd3d12CommandList);

		for (const pair<UIInfo, std::function<void(UIControlHelper&)>>& info : m_pRenderTargetList) {

			if (info.first.SceneType == SceneType) {
				//텍스쳐 있는지 없는지 확인
				if (info.first.RenderTexture) {
					info.first.RenderTexture->UpdateShaderVariable(pd3d12CommandList, 0);
				}
				if (info.first.MaskTexture) {

					info.first.MaskTexture->UpdateShaderVariable(pd3d12CommandList, 0);
				}

				//옵션
				m_pUISceneMesh[count]->UpdateMaskValue(info.first.Option);

				//SPRITEANIMATION에 따른 UV 설정
				if (info.first.EffectInfo) {
					float m_row = info.first.EffectInfo->NowRow;
					float m_rows = info.first.EffectInfo->RowNum;
					float m_col = info.first.EffectInfo->NowCol;
					float m_cols = info.first.EffectInfo->ColNum;;
					m_pUISceneMesh[count]->UpdateMaskUvCoord(UIRect{ m_row * (1.0f / m_rows) , (m_row + 1.0f) * (1.0f / m_rows) , m_col * (1.0f / m_cols), (m_col + 1.f) * (1.0f / m_cols) });
				}
				else {
					m_pUISceneMesh[count]->UpdateMaskUvCoord(UIRect(0.0f, 1.0f, 0.0f, 1.0f));
				}

				//랜더링 텍스쳐 설정
				m_pUISceneMesh[count]->UpdateVertexPosition((info.first.Rect));
				m_pUISceneMesh[count++]->Render(pd3d12CommandList);
			}
		}
	

}
// Check Ui ClickCheck
void UIManager::ClickUI(float x, float y)
{	

	float NomalizeY = 1 - ((y * 2) / FRAME_BUFFER_HEIGHT);
	float  NomalizeX = -1 + ((x * 2) / FRAME_BUFFER_WIDTH);
		//Rander Order 0 -> 1 -> 2
		for (int i = 0; i < TEXTURE_LAYER; ++i) {

			for (const pair<UIInfo, std::function<void(UIControlHelper&)>>& info : m_RenderUIList[i]) {
				if (info.first.Rect.bottom < NomalizeY && NomalizeY < info.first.Rect.top)
					if (info.first.Rect.left < NomalizeX && NomalizeX < info.first.Rect.right) {
						if (info.second) {
							m_pfunctionQueue->push_back(info.second);
						}
					}
			
			}
		}

}

void UIManager::SelectUI(float x, float y)
{

	float NomalizeY = 1 - ((y * 2) / FRAME_BUFFER_HEIGHT);
	float  NomalizeX = -1 + ((x * 2) / FRAME_BUFFER_WIDTH);
	//Rander Order 0 -> 1 -> 2
	for (int i = 0; i < TEXTURE_LAYER; ++i) {

		for (pair<UIInfo, std::function<void(UIControlHelper&)>>& info : m_RenderUIList[i]) {
			if (info.first.Rect.bottom < NomalizeY && NomalizeY < info.first.Rect.top) {
				if (info.first.Rect.left < NomalizeX && NomalizeX < info.first.Rect.right) {

					if (info.second != nullptr) {
						info.first.Option |= SELECTUI;
					} 


				}
			}
			else {
				info.first.Option &= ~SELECTUI;
			}
		}
	}

}


