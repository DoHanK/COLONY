#pragma once
#include "stdafx.h"
#include "NevMeshBaker.h"

class Cell;

class NevMesh {
public:
	//VertexBuffer
	int m_nVerties;
	XMFLOAT3* m_pxmf3Position;
	ID3D12Resource* m_pd3dVertexBuffer;
	ID3D12Resource* m_pd3dVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dPositionBufferView;

	//IndexBuffer
	int m_nIndices;
	UINT* m_pIndices;
	ID3D12Resource* m_pd3dIndexBuffer;
	ID3D12Resource* m_pd3dIndexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

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
	NevMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float WidthCellCount, float HeightCellCount, float CellSize, Cell* Cell);
	~NevMesh() {};


public:
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList);
};