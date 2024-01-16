#pragma once
#include "stdafx.h"
#include "ColonyMesh.h"
#include "ColonyGameObject.h"

struct UIInfo {
	UIRect* Rect;
	Texture* RenderTexture=NULL;
	Texture* MaskTexture=NULL;
};

#define	TEXTURE_LAYER 3
#define TEXTRUE_MASH 20
class UIManager{
public:
	UIManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,ID3D12RootSignature* pd3dGraphicsRootSignature);
	~UIManager();

	//UI Shader
	UIShader* m_Shader;

	vector<UIRectMesh*> m_pMesh;
	list<pair<UIInfo, bool(*)(void*)>> m_RenderUIList[TEXTURE_LAYER];

	UIRect CreateNormalizePixel(float top, float bottom, float left, float right);
	bool CreateUINonNormalRect(float top, float bottom, float left, float right, Texture* tex ,Texture* Masktex, bool(*f)(void* argu),int Layer);
	void AllLayerDrawRect(ID3D12GraphicsCommandList* pd3d12CommandList);

};

