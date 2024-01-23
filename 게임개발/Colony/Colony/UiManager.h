#pragma once
#include "stdafx.h"
#include "ColonyMesh.h"
#include "ColonyGameObject.h"


struct UIEffectInfo {
	float RowNum = 0;
	float ColNum = 0;
	float NowRow = 0;
	float NowCol = 0;
	float Timer = 0;
	float SetTime = 0;
};


struct UIInfo {
	UINT	SceneType;	//필수
	UIRect* Rect;		//필수
	UINT	Option;
	//선택사항
	UIEffectInfo* EffectInfo;
	Texture* RenderTexture = NULL;
	Texture* MaskTexture = NULL;
};

#define	TEXTURE_LAYER 3
#define TEXTRUE_MASH 20

#define MASKUSE 0x01 //마스크 텍스쳐 사용 여부
#define TEXTUREUSE 0x02 // 텍스쳐 사용 여부
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

	vector<UIRectMesh*> m_pMesh;
	list<pair<UIInfo, bool(*)(void*)>> m_RenderUIList[TEXTURE_LAYER];
	

	UIRect CreateNormalizePixel(float top, float bottom, float left, float right);
	bool CreateUINonNormalRect(float top, float bottom, float left, float right, Texture* tex ,Texture* Masktex, bool(*f)(void* argu),int Layer, UINT option, UINT SceneType);
	
	bool CreateUISpriteNormalRect(float top, float bottom, float left, float right, Texture* tex, Texture* Masktex, UIEffectInfo Uieffect, bool(*f)(void* argu), int Layer, UINT option, UINT SceneType);
	void AnimateUI(float ElapsedTime);

	void AllLayerDrawRect(ID3D12GraphicsCommandList* pd3d12CommandList);

};

