#include "CollisionManager.h"


void CollisionManager::LoadCollisionBoxInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList ,const char* filename)
{
	std::ifstream is{ filename,std::ios::binary };

	
	int BoxCount = 0 ;
	is.read(reinterpret_cast<char*>(&BoxCount), sizeof(int));

	for (int i = 0; i < BoxCount; ++i) {
		XMFLOAT4X4 mxf4x4Position;
		XMFLOAT3  Extend;
		is.read(reinterpret_cast<char*>(& mxf4x4Position), sizeof(float) * 16);


		is.read(reinterpret_cast<char*>(&Extend), sizeof(float) * 3);
		Extend.x *= 0.5f;
		Extend.y *= 0.5f;
		Extend.z *= 0.5f;
		BoundingOrientedBox * bxinfo = new BoundingOrientedBox(XMFLOAT3(0,0,0), Extend,XMFLOAT4(0,0,0,1));
		bxinfo->Transform(*bxinfo, DirectX::XMLoadFloat4x4(&mxf4x4Position));

		m_BoundingOrientedBoxes.push_back(bxinfo);

		BoundingBoxMesh* pBounding = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
		pBounding->UpdateVertexPosition(bxinfo);
		m_BoundingBoxMeshes.push_back(pBounding);
	}
	

	is.close();
}

void CollisionManager::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList)
{


	for (auto& RenderingBox : m_BoundingBoxMeshes) {

		RenderingBox->Render(pd3dCommandList);
	}

}
