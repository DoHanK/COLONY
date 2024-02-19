#include "stdafx.h"
#include "NevMesh.h"

NevMesh::NevMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float WidthCellCount, float HeightCellCount, float CellSize, Cell* Cell)
{
	m_WidthCellCount = WidthCellCount;
	m_HeightCellCount = HeightCellCount;
	m_CellSize = CellSize;


	//정점 총 갯수
	m_nVerties = (WidthCellCount + 1) * (HeightCellCount + 1);

	//랜더링 메쉬타입 정의
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	//Position
	m_pxmf3Position = new XMFLOAT3[m_nVerties];

	int WidthCount = m_WidthCellCount + 1;
	int HeightCount = m_HeightCellCount + 1;

	for (int y = 0; y < m_HeightCellCount; ++y) {

		for (int x = 0; x < m_WidthCellCount; ++x) {
			//격자 교점 넣기
			m_pxmf3Position[x + m_WidthCellCount * y] = XMFLOAT3(-m_WidthCellCount / 2 * m_CellSize + m_CellSize * x, 0.1f, -m_HeightCellCount / 2 * m_CellSize + m_CellSize * y);
		}
	}

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Position, m_nVerties * sizeof(XMFLOAT3), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = m_nVerties * sizeof(XMFLOAT3);

	//Index
	m_nIndices = (WidthCount * 2) * (HeightCount - 1) + ((HeightCount - 1) - 1);
	m_pIndices = new UINT[m_nIndices];

	for (int j = 0, z = 0; z < HeightCount - 1; z++) {

		//홀수번째 줄
		if ((z % 2) == 0) {

			for (int x = 0; x < WidthCount; x++) {

				if ((x == 0) && (z > 0)) m_pIndices[j++] = (UINT)(x + (z * WidthCount));

				m_pIndices[j++] = (UINT)(x + (z * WidthCount));
				m_pIndices[j++] = (UINT)(x + (z * WidthCount) + WidthCount);
			}
		}
		//짝수번째 줄 
		else {

			for (int x = WidthCount - 1; x >= 0; x--) {

				if (x == (WidthCount - 1)) m_pIndices[j++] = (UINT)(x + (z * WidthCount));
				m_pIndices[j++] = (UINT)(x + (z * WidthCount));
				m_pIndices[j++] = (UINT)(x + (z * WidthCount) + WidthCount);
			}
		}

	}

	m_pd3dIndexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = m_nIndices * sizeof(UINT);

	m_Pass = new UINT[m_nVerties * 2];
	//Color
	for (int y = 0, z = 0; y < m_HeightCellCount; y++) {

		for (int x = 0; x < m_WidthCellCount; x++) {

			if (Cell[x + y * m_WidthCellCount].m_Pass) {
				m_Pass[z++] = 1;
				m_Pass[z++] = 1;
			}
			else {
				m_Pass[z++] = 0;
				m_Pass[z++] = 0;
			}
		}
	}
		
	m_pd3dPassBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Position, m_nVerties * 2 * sizeof(UINT), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPassUploadBuffer);

	m_d3dPassBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dPassBufferView.StrideInBytes = sizeof(UINT);
	m_d3dPassBufferView.SizeInBytes = m_nVerties * 2 * sizeof(UINT);


}

void NevMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	OnPreRender(pd3dCommandList);

	pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
	pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
}

void NevMesh::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[2] = { m_d3dPositionBufferView,m_d3dPassBufferView };
	pd3dCommandList->IASetVertexBuffers(0, 2, pVertexBufferViews);
}
