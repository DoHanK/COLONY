#pragma once
#include "stdafx.h"
#include "ColonyMesh.h"


class CollisionManager{
public:
	
	std::vector<BoundingOrientedBox*> m_BoundingOrientedBoxes;
	std::vector<BoundingBoxMesh*> m_BoundingBoxMeshes;
	

	void LoadCollisionBoxInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* filename);

	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList);
};

