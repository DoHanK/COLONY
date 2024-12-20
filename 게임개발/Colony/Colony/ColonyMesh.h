#pragma once
#include "stdafx.h"

#define VERTEXT_POSITION				0x0001
#define VERTEXT_COLOR					0x0002
#define VERTEXT_NORMAL					0x0004
#define VERTEXT_TANGENT					0x0008
#define VERTEXT_TEXTURE_COORD0			0x0010
#define VERTEXT_TEXTURE_COORD1			0x0020

#define VERTEXT_BONE_INDEX_WEIGHT		0x1000

#define VERTEXT_TEXTURE					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_DETAIL					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TEXTURE			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_TANGENT_TEXTURE	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_DETAIL			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TANGENT__DETAIL	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)

class GameObject;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										BasicMesh Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BasicMesh{
public:
	BasicMesh() {};
	BasicMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~BasicMesh();
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	char							m_pstrMeshName[64] = { 0 };

protected:
	UINT							m_nType = 0x00;

	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);
	BoundingOrientedBox				m_BoundingBox = BoundingOrientedBox();
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;

protected:
	int								m_nVertices = 0;

	XMFLOAT3*						m_pxmf3Positions = NULL;

	ID3D12Resource*					m_pd3dPositionBuffer = NULL;
	ID3D12Resource*					m_pd3dPositionUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	int								m_nSubMeshes = 0;
	int*							m_pnSubSetIndices = NULL;
	UINT**							m_ppnSubSetIndices = NULL;

	ID3D12Resource**				m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource**				m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW*		m_pd3dSubSetIndexBufferViews = NULL;

public:
	UINT GetType() { return(m_nType); }
	virtual BoundingOrientedBox& GetBoundingBox() { return m_BoundingBox; };
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) { }
	virtual void ReleaseShaderVariables() { }
	virtual const XMFLOAT3& GetAABBCenter() { return m_xmf3AABBCenter; };
	virtual const XMFLOAT3& GetAABBExtend() { return m_xmf3AABBExtents; };
	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
	virtual void OnPostRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										StandardMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class StandardMesh : public BasicMesh {
public:
	StandardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~StandardMesh();

protected:
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT3* m_pxmf3Normals = NULL;
	XMFLOAT3* m_pxmf3Tangents = NULL;
	XMFLOAT3* m_pxmf3BiTangents = NULL;
	XMFLOAT2* m_pxmf2TextureCoords0 = NULL;
	XMFLOAT2* m_pxmf2TextureCoords1 = NULL;

	ID3D12Resource* m_pd3dTextureCoord0Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord0UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource* m_pd3dTextureCoord1Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord1UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord1BufferView;

	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

	ID3D12Resource* m_pd3dTangentBuffer = NULL;
	ID3D12Resource* m_pd3dTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTangentBufferView;

	ID3D12Resource* m_pd3dBiTangentBuffer = NULL;
	ID3D12Resource* m_pd3dBiTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBiTangentBufferView;

public:
	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);


};


#define SKINNED_ANIMATION_BONES		128
/////////////////////////////////////////////////////////////////////////////////////////////////////
//									SkinnedMesh Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class SkinnedMesh : public StandardMesh {
public:
	SkinnedMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~SkinnedMesh();

protected:
	int								m_nBonesPerVertex = 4;

	XMINT4* m_pxmn4BoneIndices = NULL;
	XMFLOAT4* m_pxmf4BoneWeights = NULL;

	ID3D12Resource* m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource* m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	ID3D12Resource* m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource* m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0;

	char(*m_ppstrSkinningBoneNames)[64];
	GameObject** m_ppSkinningBoneFrameCaches = NULL; //[m_nSkinningBones]

	XMFLOAT4X4* m_pxmf4x4BindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource* m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL;

	ID3D12Resource* m_pd3dcbSkinningBoneTransforms = NULL; //Pointer Only, Resource from CAnimationController
	XMFLOAT4X4* m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(GameObject* pModelRootObject);

	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);


};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										DynamicMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class DynamicMesh {
public:
	DynamicMesh() {};
	virtual ~DynamicMesh() { };
protected:

	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nOffset = 0;
	UINT							m_nType = 0;
	UINT							m_nStride = 0;

public:
	XMFLOAT3*						m_pVertices = NULL;
	UINT*							m_pnIndices = NULL;
	int								m_nIndices = 0;



	ID3D12Resource*					m_pd3dPositionBuffer;
	UINT							m_nVertexBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferViews;

	XMFLOAT3*						m_pcbMappedPositions = NULL;
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {};
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										UIRectMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct UIRect {
	// -1 ~ 1 nomalize value
	float top;
	float bottom;
	float left;
	float right;
};

class UIRectMesh :public DynamicMesh{
public:
	UIRectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~UIRectMesh();

	//텍스쳐 매핑을 위한 UV좌표
	ID3D12Resource*					m_pd3dUvBuffer;
	UINT							m_nUvBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dUvBufferViews;
	XMFLOAT2*						m_pcbMappedUvs;

	//텍스쳐 매핑을 위한 MASK UV좌표
	ID3D12Resource*					m_pd3dMaskUvBuffer;
	UINT							m_nMaskUvBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dMaskUvBufferViews;
	XMFLOAT2*						m_pcbMappedMaskUvs;

	//MaskValue
	ID3D12Resource*					m_pd3dMaskBuffer;
	UINT							m_nMaskBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dMaskBufferViews;
	UINT*							m_pcbMappedMasks;

	//위치 지정해주기
	void UpdateVertexPosition(const UIRect& Rect);
	void UpdateUvCoord(const UIRect& Rect);
	void UpdateMaskUvCoord(const UIRect& Rect);
	//Mask Setting
	void UpdateMaskValue(const UINT& lefttop, const UINT& righttop, const UINT& leftbottom, const UINT& rightbottom);
	void UpdateMaskValue(const UINT& AllRectMask);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									AABBBoundingMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class BoundingBoxMesh : public DynamicMesh
{
public:
	BoundingBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~BoundingBoxMesh();

	XMFLOAT3* m_pcbMappedPositions = NULL;

	void UpdateVertexPosition(BoundingOrientedBox* pxmBoundingBox);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									Shere Class													   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class ShphereMesh : public BasicMesh
{
public:
	ShphereMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nslice, int nstack, float radian);
	virtual ~ShphereMesh() {};


	int m_nSlices = 30;
	int	m_nStacks = 60;
	float m_fRadius;


};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										SkyBoxMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class SkyBoxMesh :public BasicMesh
{
public:
	SkyBoxMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~SkyBoxMesh();
};





/////////////////////////////////////////////////////////////////////////////////////////////////////
//										BillboardMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class BillBoardMesh :public BasicMesh {
public:
	BillBoardMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~BillBoardMesh();
	void UpdateUvCoord(UIRect Rect);
	void UpdataVertexPosition(UIRect Rect, float z);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
public:
	//position
	XMFLOAT3*						m_pcbMappedPositions = NULL;
	UINT							m_nVertexBufferViews;
	D3D12_VERTEX_BUFFER_VIEW*		m_pd3dVertexBufferViews;
	//normal
	ID3D12Resource*					m_pd3dNormalBuffer;
	UINT							m_nNormalBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW*		m_pd3dNormalBufferViews;
	ID3D12Resource*					m_pd3dNormalUploadBuffer = NULL;
	//uv
	ID3D12Resource*					m_pd3dUvBuffer;
	UINT							m_nUvBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW*		m_pd3dUvBufferViews;
	ID3D12Resource*					m_pd3dUvUploadBuffer = NULL;
	XMFLOAT2*						m_pcbMappedUvs = NULL;
};




/////////////////////////////////////////////////////////////////////////////////////////////////////
//										ParticleMesh Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ParticleVertex {
public:
	XMFLOAT3		m_f3position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	UINT			m_type = 0;
	XMFLOAT3		m_f3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3		m_f3acceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float			m_speed = 0.0f;
	float			m_lifeTime = 0.0f;
	float			m_age = 0.0f;
	float			m_startTime = 0.0f;
public:
	ParticleVertex() {}
	~ParticleVertex() {}
};

class ParticleMesh : public BasicMesh {
public:
	ParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 position, UINT type, XMFLOAT3 direction, float speed, float lifeTime, float age, float startTime, XMFLOAT3 acceleration, UINT nMaxParticles);
	virtual ~ParticleMesh();

	void CreateVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 position, UINT type, XMFLOAT3 direction, float speed, float lifeTime, float age, float startTime, XMFLOAT3 acceleration);
	void CreateStreamOutputBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nMaxParticles);

	void RenderStreamOutput(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList); // 1st 렌더링 (stream output)
	void RenderDrawBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);   // 2nd 렌더링 
	void PostRender(int nPipelineState);
public:
	UINT								m_nStride = 0;
	ID3D12Resource* m_vertexBuffer;
	ID3D12Resource* m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_vertexBufferView;

	UINT								m_nMaxParticles = 0;
	ID3D12Resource* m_streamOutputBuffer;
	D3D12_STREAM_OUTPUT_BUFFER_VIEW		m_streamOutputBufferView;
	ID3D12Resource* m_drawBuffer;
	ID3D12Resource* m_defaultBufferFileSize;
	ID3D12Resource* m_uploadBufferFileSize;
	ID3D12Resource* m_readBackBufferFileSize;
	UINT64* m_pUploadBufferFillSize;

	bool								m_start = true;
};