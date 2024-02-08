#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"

class QuadTree
{public:
	QuadTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int depth, XMFLOAT3 center, XMFLOAT3 Extend);
	~QuadTree() {};
public:
	int							m_depth;
	BoundingBox					m_BoundingBox;

	std::vector<GameObject*>	m_StaticObject;
	std::vector<GameObject*>	m_DynamicObject;



	QuadTree* m_LeftUp = NULL;
	QuadTree* m_RightUp = NULL;
	QuadTree* m_LeftBottom = NULL;
	QuadTree* m_RightBottom = NULL;

	BoundingBoxMesh* m_BoundingMesh =NULL;
public:
	bool BuildTreeByDepth(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int limitdepth);

	void InsertStaticObject();

	void InsertDynamicObject();

	void BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList,int DepthLevel);
};

