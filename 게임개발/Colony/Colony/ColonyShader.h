#pragma once
#include "stdafx.h"
#include "ColonyCamera.h"
#include "ColonyGameObject.h"

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

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dPrimitiveTopology, UINT nRenderTargets, DXGI_FORMAT* pdxgiRtvFormats, DXGI_FORMAT dxgiDsvFormat);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World) { }

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);

	virtual void ReleaseUploadBuffers() { }

	//virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, void* pContext = NULL) { }
	virtual void AnimateObjects(float fTimeElapsed) { }
	virtual void ReleaseObjects() { }

protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	ID3D12PipelineState* m_pd3dPipelineState = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_d3dPipelineStateDesc;

	float								m_fElapsedTime = 0.0f;
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
//										 DepthSkinnedRenderingShader Class		  					//
/////////////////////////////////////////////////////////////////////////////////////////////////////


class DepthSkinnedRenderingShader : public BasicShader
{
public:
	DepthSkinnedRenderingShader();
	virtual ~DepthSkinnedRenderingShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 DepthRenderingShader Class		  			     		    //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class DepthRenderingShader :public DepthSkinnedRenderingShader {
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 BillboardShader Class		  				    		   //
/////////////////////////////////////////////////////////////////////////////////////////////////////


class BillboardShader :public BasicShader
{
public:
	BillboardShader();
	~BillboardShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader();
	virtual D3D12_SHADER_BYTECODE CreatePixelShader();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										 ParticleShader Class		  				    		   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleShader :public BasicShader {
public:
	ParticleShader();
	~ParticleShader();

	D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);
	D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	D3D12_STREAM_OUTPUT_DESC CreateStreamOuputState(int nPipelineState);
	D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState);
	void RenderShader(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, int nPipelineState);
public:
	UINT								m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
};