#pragma once
#include "stdafx.h" 
#include "ColonyMesh.h"


class PerceptionRangeMesh:public BasicMesh{
public:
	PerceptionRangeMesh(ID3D12Device* pd3dDeivce, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~PerceptionRangeMesh();
public:
	int m_nSlices		= 30;
	int	m_nStacks		= 60;
	float m_fRadius		= AISIGHTRANGE;
	float m_fov			= AIFOV * XM_PI / 180.0f;
	float m_fovHeight	= AIFOVHEIGHT;
//Fov 
public:
	XMFLOAT3* m_pVertices = NULL;
	UINT* m_pnIndices = NULL;
	int								m_nIndices = 0;

	int								m_nFovVertices = 0;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dFOVPrimitiveTopology;
	ID3D12Resource*					m_pd3dFOVPositionBuffer;
	UINT							m_nVertexFOVBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexFOVBufferViews;

	int								m_nStride = 0;
	XMFLOAT3*						m_pcbMappedFOVPositions = NULL;

	virtual void PerceptionRangeRender(ID3D12GraphicsCommandList* pd3dCommandList, int nSubSet);
	virtual void PerceptionBindRender(ID3D12GraphicsCommandList* pd3dCommandList);

};

