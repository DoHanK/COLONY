#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"
class Cell {
public:
	BoundingOrientedBox m_BoundingBox;
	bool m_Pass;
	BoundingBoxMesh* m_pBoundingMesh;
};

class NevMeshBaker
{
public:
	NevMeshBaker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float CellSize, float WidthLengh, float HeightLengh);
	~NevMeshBaker() {};

public:
	Cell* m_Grid;

	float m_CellSize;
	//Half±Ê¿Ã
	float m_WidthLengh;
	float m_HeightLengh;

	int m_WidthCount;
	int m_HeightCount;
	void BakeNevMeshByObject(const std::vector<GameObject>& StaticObstacle);
	void BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList);
};

