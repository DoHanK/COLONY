#pragma once
#include "stdafx.h"
#include "ColonyMesh.h"
#include "ColonyGameObject.h"
#include "UIControlHelper.h"

class UIControlHelper;

struct UIEffectInfo {
	float RowNum = 0;
	float ColNum = 0;
	float NowRow = 0;
	float NowCol = 0;
	float Timer = 0;
	float SetTime = 0;
};


struct UIInfo {
	UINT	SceneType;	//�ʼ�
	UIRect Rect;		//�ʼ�
	UINT	Option;
	//���û���
	UIEffectInfo* EffectInfo;
	Texture* RenderTexture = NULL;
	Texture* MaskTexture = NULL;
};

#define	TEXTURE_LAYER 3
#define TEXTRUE_MASH 20

#define MASKUSE 0x01 //����ũ �ؽ��� ��� ����
#define TEXTUREUSE 0x02 // �ؽ��� ��� ����
#define AMPLIFIER 0x04

/////////////////////////////////////////////////////////////////////////////////////////////////////
//											UIManager Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class UIManager{
public:
	UIManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,ID3D12RootSignature* pd3dGraphicsRootSignature);
	~UIManager();

	//UI Shader
	UIShader* m_Shader;
	std::deque<std::function<void(UIControlHelper&)>>* m_pfunctionQueue;

	vector<UIRectMesh*> m_pUIMesh;
	list<pair<UIInfo, std::function<void(UIControlHelper&)>>> m_RenderUIList[TEXTURE_LAYER];
	vector<UIRectMesh*> m_pUISceneMesh;
	list<pair<UIInfo, std::function<void(UIControlHelper&)>>> m_pRenderTargetList;

	UIRect CreateNormalizePixel(float top, float bottom, float left, float right);
	UIInfo* CreateUINonNormalRect(float top, float bottom, float left, float right, Texture* tex ,Texture* Masktex, std::function<void(UIControlHelper&)>,int Layer, UINT option, UINT SceneType,bool bNormal);
	
	UIInfo* CreateUISpriteNormalRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, UIEffectInfo Uieffect, std::function<void(UIControlHelper&)>, int Layer, UINT option, UINT SceneType, bool bNormal);
	void AnimateUI(float ElapsedTime);
	void AllLayerDrawRect(ID3D12GraphicsCommandList* pd3d12CommandList, int SceneType);

	UIInfo* CreateRederTargetRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, std::function<void(UIControlHelper&)> f, int Layer, UINT option, UINT SceneType, bool bNormal);
	void DrawScene(ID3D12GraphicsCommandList* pd3d12CommandList, int SceneType);

	void ClickUI(float x, float y);
};

