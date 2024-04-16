#include "stdafx.h"
#include "NevMeshBaker.h"
#include "ColonyMesh.h"
#include "NevMesh.h"


NevMeshBaker::NevMeshBaker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,float CellSize, float Width, float Height , bool LoadCell ): m_CellSize(CellSize), m_Width(Width), m_Height(Height)
{		
	//Cell의 가로 갯수, 세로 갯수
	m_WidthCount =  ceil(2*Width / CellSize);
	m_HeightCount = ceil(2*Height / CellSize);

	if(LoadCell){
		LoadNevMesh();
	}
	else {
		m_Grid.resize(m_WidthCount * m_HeightCount);
		for (int y = 0; y < m_HeightCount; y++) {

			for (int x = 0; x < m_WidthCount; x++) {

				m_Grid[m_WidthCount * y + x].m_Pass = true;
				m_Grid[m_WidthCount * y + x].m_BoundingBox.Center = XMFLOAT3(-m_Width + m_CellSize * x + m_CellSize / 2, 0.1f, -m_Height + m_CellSize * y + m_CellSize / 2);
				m_Grid[m_WidthCount * y + x].m_BoundingBox.Extents = XMFLOAT3(CellSize / 2, 0.1f, CellSize / 2);

				//이전 방식
				//m_Grid[m_WidthCount * y + x].m_pBoundingMesh = new  BoundingBoxMesh(pd3dDevice, pd3dCommandList);
				//m_Grid[m_WidthCount * y + x].m_pBoundingMesh->UpdateVertexPosition(&m_Grid[m_WidthCount * y + x].m_BoundingBox);
			}

		}
	}
	
	m_pMesh = new NevMesh(pd3dDevice, pd3dCommandList, m_Width, m_Height, m_CellSize, &m_Grid[0]);
	m_pMesh->AddRef();
}

NevMeshBaker::~NevMeshBaker() {

	if(m_pMesh) m_pMesh->Release();


}

void NevMeshBaker::ReleaseUploadBuffers() {
	if(m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void NevMeshBaker::SaveNevMesh(){

	std::ofstream os{"NevCell.bin",std::ios::binary};

	int Gridsize = m_Grid.size();

	os.write(reinterpret_cast<char*>(&Gridsize), sizeof(int));

	os.write(reinterpret_cast<char*>(&m_Grid[0]), sizeof(Cell) * m_Grid.size());
	
	os.close();
}

void NevMeshBaker::LoadNevMesh()
{
	std::ifstream is{ "NevCell.bin",std::ios::binary };

	int Gridsize;

	is.read(reinterpret_cast<char*>(&Gridsize), sizeof(int));

	m_Grid.resize(Gridsize);
	is.read(reinterpret_cast<char*>(&m_Grid[0]), sizeof(Cell) * m_Grid.size());

	is.close();

}

void NevMeshBaker::BakeNevMeshByObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::vector<GameObject*>& StaticObstacle)
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


}

void NevMeshBaker::BakeNevMeshByCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::vector<Collision*>& StaticObstacle)
{
	int count = 0;
	for (int y = 0; y < m_HeightCount; y++) {

		for (int x = 0; x < m_WidthCount; x++) {

			for (const auto& Object : StaticObstacle) {

				if (((BOBBox*)Object)->m_boundingbox.Extents.x > 200.f) continue;

				if (((BOBBox*)Object)->m_boundingbox.Intersects(m_Grid[m_WidthCount * y + x].m_BoundingBox)) {
					m_Grid[m_WidthCount * y + x].m_Pass = false;
				}

			}
			if (!m_Grid[m_WidthCount * y + x].m_Pass) {
				count++;

			}
		}

	}
}

void NevMeshBaker::BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	
	if(m_pMesh) m_pMesh->Render(pd3dCommandList);
}
