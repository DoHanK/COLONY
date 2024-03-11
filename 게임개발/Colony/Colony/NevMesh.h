#pragma once
#include "stdafx.h"
#include "NevMeshBaker.h"

class Cell;

class NevMesh {

public:
	NevMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Width, float Height, float CellSize, Cell* Cell);
	~NevMesh();
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	//VertexBuffer
	int m_nVerties;
	XMFLOAT3* m_pxmf3Position;
	ID3D12Resource* m_pd3dVertexBuffer;
	ID3D12Resource* m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	//Determine
	UINT* m_Pass;
	ID3D12Resource* m_pd3dPassBuffer;
	ID3D12Resource* m_pd3dPassUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPassBufferView;


	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;

	int m_WidthCellCount;
	int m_HeightCellCount;
	float m_CellSize;

public:
	void ReleaseUploadBuffers();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
};

class RouteMesh {
public:
	RouteMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~RouteMesh();
private:
	int								m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	int m_nVertices;
	int m_nStride;
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	XMFLOAT3* m_pVertices = NULL;
	
	ID3D12Resource*					m_pd3dPositionBuffer;
	UINT							m_nVertexBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferViews;

	XMFLOAT3* m_pcbMappedPositions = NULL;
public:
	void Update(std::list<XMFLOAT2> path);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
};