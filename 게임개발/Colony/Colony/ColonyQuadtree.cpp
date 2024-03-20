#include "ColonyQuadtree.h"


QuadTree::QuadTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList ,int depth, XMFLOAT3 center, XMFLOAT3 Extend) :m_depth(depth), m_BoundingBox(BoundingBox(center, Extend))
{
	m_BoundingMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
	m_BoundingMesh->UpdateVertexPosition(&BoundingOrientedBox(m_BoundingBox.Center, m_BoundingBox.Extents, XMFLOAT4(0, 0, 0, 1)));
}
QuadTree::~QuadTree()
{
	m_BoundingMesh->Release();
}
void QuadTree::AddRef()
{
	if (m_LeftUp) m_LeftUp->AddRef();
	if (m_RightUp) m_RightUp->AddRef();
	if (m_LeftBottom) m_LeftBottom->AddRef();
	if (m_RightBottom) m_RightBottom->AddRef();
	this->AddRef();
}
void QuadTree::Release(){
	if (m_LeftUp) m_LeftUp->Release();
	if (m_RightUp) m_RightUp->Release();
	if (m_LeftBottom) m_LeftBottom->Release();
	if (m_RightBottom) m_RightBottom->Release();

	if (--m_nReferences <= 0) delete this;
}
;

bool QuadTree::BuildTreeByDepth(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int limitdepth)
{

	if (limitdepth < m_depth) {
		return false;
	}

	XMFLOAT3 Center = m_BoundingBox.Center;
	XMFLOAT3 Extend = m_BoundingBox.Extents;
	float HalfExtendx = Extend.x/2;
	float HalfExtendy = Extend.y/2;
	float HalfExtendz = Extend.z/2;

	if (!m_LeftUp) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x - HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z + HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y , HalfExtendz);

		m_LeftUp = new QuadTree(pd3dDevice, pd3dCommandList, m_depth+1, LUCenter, LUExtend);

		m_LeftUp->BuildTreeByDepth(pd3dDevice, pd3dCommandList,limitdepth);
	}

	if (!m_RightUp) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x + HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z + HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_RightUp = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_RightUp->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

	if (!m_LeftBottom) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x - HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z - HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_LeftBottom = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_LeftBottom->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

	if (!m_RightBottom) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x + HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z - HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_RightBottom = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_RightBottom->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

}

void QuadTree::InsertStaticObject()
{
}

void QuadTree::InsertDynamicObject()
{
}

void QuadTree::BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList,int DepthLevel){

	if (m_BoundingMesh && (DepthLevel == m_depth) ) {
		m_BoundingMesh->Render(pd3dCommandList);
	}
	//ChildNode
	if (m_LeftUp)
		m_LeftUp->BoundingRendering(pd3dCommandList, DepthLevel);
	if(m_RightUp)
		m_RightUp->BoundingRendering(pd3dCommandList, DepthLevel);
	if(m_LeftBottom)
		m_LeftBottom->BoundingRendering(pd3dCommandList, DepthLevel);
	if(m_RightBottom)
		m_RightBottom->BoundingRendering(pd3dCommandList, DepthLevel);

}


