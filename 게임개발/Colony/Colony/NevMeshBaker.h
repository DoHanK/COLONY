#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"
#include "NevMesh.h"

class NevMesh;

class Cell {
public:
	BoundingOrientedBox m_BoundingBox;
	bool m_Pass;
	BoundingBoxMesh* m_pBoundingMesh;
};

class NevMeshBaker
{
public:
	NevMeshBaker(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float CellSize, float WidthLengh, float HeightLengh, bool LoadCell);
	~NevMeshBaker();

public:

	std::vector<Cell> m_Grid;
	

	float m_CellSize;
	//Half±Ê¿Ã
	float m_Width;
	float m_Height;


	int m_WidthCount;
	int m_HeightCount;
	NevMesh* m_pMesh =NULL;
public:
	void SaveNevMesh();
	void LoadNevMesh();
	void ReleaseUploadBuffers();
	void BakeNevMeshByObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::vector<GameObject*>& StaticObstacle);
	void BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList);
};

