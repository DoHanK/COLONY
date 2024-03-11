#include "stdafx.h"
#include "NevMesh.h"

NevMesh::NevMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float Width, float Height, float CellSize, Cell* Cell)
{
	m_WidthCellCount = ceil(2 * Width / CellSize);  
	m_HeightCellCount = ceil(2 * Height / CellSize);  
	m_CellSize = CellSize;


	//정점 총 갯수
	m_nVerties = (m_WidthCellCount) * (m_HeightCellCount) * 24;

	//랜더링 메쉬타입 정의
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	//Position
	m_pxmf3Position = new XMFLOAT3[m_nVerties];
	m_Pass = new UINT[m_nVerties];

	//Color
	for (int y = 0, z = 0 ,j = 0; y < m_HeightCellCount; y++) {

		for (int x = 0; x < m_WidthCellCount; x++) {

			XMFLOAT3 xmf3Corners[8];
			Cell[x + y * m_WidthCellCount].m_BoundingBox.GetCorners(xmf3Corners);

			m_pxmf3Position[z++] = xmf3Corners[0];
			m_pxmf3Position[z++] = xmf3Corners[1];

			m_pxmf3Position[z++] = xmf3Corners[1];
			m_pxmf3Position[z++] = xmf3Corners[2];

			m_pxmf3Position[z++] = xmf3Corners[2];
			m_pxmf3Position[z++] = xmf3Corners[3];

			m_pxmf3Position[z++] = xmf3Corners[3];
			m_pxmf3Position[z++] = xmf3Corners[0];

			m_pxmf3Position[z++] = xmf3Corners[4];
			m_pxmf3Position[z++] = xmf3Corners[5];

			m_pxmf3Position[z++] = xmf3Corners[5];
			m_pxmf3Position[z++] = xmf3Corners[6];

			m_pxmf3Position[z++] = xmf3Corners[6];
			m_pxmf3Position[z++] = xmf3Corners[7];

			m_pxmf3Position[z++] = xmf3Corners[7];
			m_pxmf3Position[z++] = xmf3Corners[4];

			m_pxmf3Position[z++] = xmf3Corners[0];
			m_pxmf3Position[z++] = xmf3Corners[4];

			m_pxmf3Position[z++] = xmf3Corners[1];
			m_pxmf3Position[z++] = xmf3Corners[5];

			m_pxmf3Position[z++] = xmf3Corners[2];
			m_pxmf3Position[z++] = xmf3Corners[6];

			m_pxmf3Position[z++] = xmf3Corners[3];
			m_pxmf3Position[z++] = xmf3Corners[7];

			if (Cell[x + y * m_WidthCellCount].m_Pass) {

		
				for (int i = 0; i < 24; ++i) {
					m_Pass[j++] = 1;
				}


			}
			else {
				
				for (int i = 0; i < 24; ++i) {
					m_Pass[j++] = 0;
				}

			}
		}
	}



	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Position, m_nVerties * sizeof(XMFLOAT3), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = m_nVerties * sizeof(XMFLOAT3);
		
	m_pd3dPassBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_Pass, m_nVerties * sizeof(UINT), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPassUploadBuffer);

	m_d3dPassBufferView.BufferLocation = m_pd3dPassBuffer->GetGPUVirtualAddress();
	m_d3dPassBufferView.StrideInBytes = sizeof(UINT);
	m_d3dPassBufferView.SizeInBytes = m_nVerties  * sizeof(UINT);


}

NevMesh::~NevMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dPassBuffer) m_pd3dPassBuffer->Release();
}

void NevMesh::ReleaseUploadBuffers()
{
	if (m_pd3dPassUploadBuffer) m_pd3dPassUploadBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pxmf3Position) {
		delete[] m_pxmf3Position;
		m_pxmf3Position = NULL;	
	}
	if (m_Pass) {
		delete[] m_Pass;
		m_Pass = NULL;
	}
}

void NevMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPreRender(pd3dCommandList);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->DrawInstanced(m_nVerties, 1, 0, 0);
}

void NevMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView,m_d3dPassBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferViews);
}

RouteMesh::RouteMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_nVertices = ShowNodeNum;
	m_nStride = sizeof(XMFLOAT3);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dPositionBuffer->Map(0, NULL, (void**)&m_pcbMappedPositions);

	m_nVertexBufferViews = 1;

	m_d3dVertexBufferViews.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferViews.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferViews.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

}

RouteMesh::~RouteMesh()
{
	if (m_pd3dPositionBuffer) {
		m_pd3dPositionBuffer->Unmap(0, NULL);
		m_pd3dPositionBuffer->Release();
	}
}

void RouteMesh::Update(std::list<XMFLOAT2> path)
{
	int i = 0;
	for (const auto& pos : path) {
		if (i > m_nVertices) break;
			m_pcbMappedPositions[i++] = XMFLOAT3(pos.x, 0.01f, pos.y);
	}

	for (; i < m_nVertices; ++i) {
		m_pcbMappedPositions[i] = XMFLOAT3(path.back().x, 0.01f, path.back().y);
	}
}

void RouteMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{

	OnPreRender(pd3dCommandList);
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	pd3dCommandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

void RouteMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{

	pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferViews);
}
