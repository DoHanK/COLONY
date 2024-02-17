#include "NevMeshBaker.h"
#include "ColonyMesh.h"

NevMeshBaker::NevMeshBaker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,float CellSize, float Width, float Height ): m_CellSize(CellSize), m_WidthLengh(Width), m_HeightLengh(Height)
{		
	//CellÀÇ °¡·Î °¹¼ö, ¼¼·Î °¹¼ö
	m_WidthCount = ceil(2*Width / CellSize);
	m_HeightCount = ceil(2*Height / CellSize);

	m_Grid = new Cell[m_WidthCount * m_HeightCount];

	for (int y = 0; y < m_HeightCount; y++) {

		for (int x = 0; x < m_WidthCount; x++) {

			m_Grid[m_WidthCount * y + x].m_Pass = true;
			m_Grid[m_WidthCount * y + x].m_BoundingBox.Center = XMFLOAT3(-Width + m_CellSize * x + m_CellSize / 2, 0.1f, -m_HeightLengh + m_CellSize * y + m_CellSize / 2);
			m_Grid[m_WidthCount * y + x].m_BoundingBox.Extents = XMFLOAT3(CellSize/2, 0.1f, CellSize/2);
			m_Grid[m_WidthCount * y + x].m_pBoundingMesh = new  BoundingBoxMesh(pd3dDevice, pd3dCommandList);
			m_Grid[m_WidthCount * y + x].m_pBoundingMesh->UpdateVertexPosition(&m_Grid[m_WidthCount * y + x].m_BoundingBox);
		}
		
	}

}

void NevMeshBaker::BakeNevMeshByObject(const std::vector<GameObject*>& StaticObstacle)
{
	int count = 0;
	for (int y = 0; y < m_HeightCount; y++) {

		for (int x = 0; x < m_WidthCount; x++) {

			for (const auto& Object : StaticObstacle) {

				if (strstr(Object->m_pstrFrameName, "Plane") != NULL) {
					continue;
				}

				if (Object->m_BoundingBox.Intersects(m_Grid[m_WidthCount * y + x].m_BoundingBox)) {
					m_Grid[m_WidthCount * y + x].m_Pass = false;
				}
		
			}
			if (!m_Grid[m_WidthCount * y + x].m_Pass) {
				count++;

			}
		}

	}

	OutputDebugStringA(to_string(count).c_str());

}

void NevMeshBaker::BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	for (int y = 0; y < m_HeightCount; y++) {

		for (int x = 0; x < m_WidthCount; x++) {
			XMFLOAT3 xmfloat3(1.0f, 0.0f, 0);
			if (m_Grid[m_WidthCount * y + x].m_Pass) {
	
				xmfloat3 = XMFLOAT3(1.0f, 1.0f, 1.0f);
		
			}

			pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
			m_Grid[m_WidthCount * y + x].m_pBoundingMesh->Render(pd3dCommandList);
		}

	}
}
