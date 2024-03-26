#pragma once
#include "stdafx.h"
#include "ColonyCamera.h"
#include "ColonyGameObject.h"
#include "ColonyScene.h"
#include "ResourceManager.h"

class Camera;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										BasicShader Class				  						   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BasicShader
{
public:
	BasicShader();
	virtual ~BasicShader();

private:
	int									m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob = NULL);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera=NULL);

	virtual void ReleaseUploadBuffers() { }

	//virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

	void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, Texture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	

protected:


	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	ID3D12PipelineState* m_pd3dPipelineState = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

	float								m_fElapsedTime = 0.0f;

	ID3D12DescriptorHeap*			m_pd3dCbvSrvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dSrvCPUDescriptorNextHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGPUDescriptorNextHandle;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//							SkinnedAnimationStandardShader Class				  				   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class StandardShader : public BasicShader
{
public:
	StandardShader();
	virtual ~StandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										StandardShader Class				  					   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class SkinnedAnimationStandardShader : public BasicShader
{
public:
	SkinnedAnimationStandardShader();
	virtual ~SkinnedAnimationStandardShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										StandardShader Class				  					   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class GhostTraillerShader :public BasicShader {
public:
	GhostTraillerShader();
	virtual ~GhostTraillerShader();


	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										UIShader Class				  							   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class UIShader :public BasicShader {
public:
	UIShader();
	virtual ~UIShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										Bounding Class				  							   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BoundingShader : public BasicShader
{
public:
	BoundingShader();
	virtual ~BoundingShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
};




/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 Plane Class				  							   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class PlaneShader :public BasicShader
{
public:
	PlaneShader();
	virtual ~PlaneShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 SkyBoxShader Class				  			     		   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class SkyBoxShader :public BasicShader
{
public:
	SkyBoxShader();
	virtual ~SkyBoxShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();


};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 NevMeshShader Class				  			     	   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class NevMeshShader :public BoundingShader
{
public:
	NevMeshShader();
	virtual ~NevMeshShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();


};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 ShadowMapRenderShader Class		  			     	   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct LIGHT;


struct LIGHTSPACEINFO
{
	XMFLOAT4X4						m_matToTextures[MAX_LIGHTS]; //Transposed
	XMFLOAT4						m_LightPositions[MAX_LIGHTS];
};

class ShadowMapRenderShader :public StandardShader {
public:
	ShadowMapRenderShader(LIGHT* Lights);
	virtual ~ShadowMapRenderShader();

	//virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	//StandardShader 와 같은 버텍스 쉐이더 사용
	//virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob=NULL);
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	D3D12_RASTERIZER_DESC CreateRasterizerState();

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	void MakeShadowMap(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void ReleaseObjects();
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);

public:
	XMMATRIX			m_matProjectionToTexture;
	LIGHT*				m_pLights;
	LIGHTSPACEINFO*		m_pLightSpaceInfo;
	ID3D12Resource*		m_pd3dcbToLightSpaces = NULL;
	LIGHTSPACEINFO*		m_pcbMappedToLightSpaces = NULL;

	Camera*				m_ppShadowMapRenderCamera[MAX_DEPTH_TEXTURES];
	Texture*			m_pShadowMapTexture;
	GamePlayScene*		m_PlayScene;


	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dRtvCPUDescriptorHandles[MAX_DEPTH_TEXTURES];
	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;
	ID3D12Resource* m_pd3dDepthBuffer = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;
};

