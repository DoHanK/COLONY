#include "ColonyQuadtree.h"


mutex test;

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

bool QuadTree::BuildTreeByDepth(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int limitdepth){

	if (limitdepth <= m_depth) {
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

		m_LeftUp->m_Route = this->m_Route;
		m_LeftUp->m_Route.push_back(LEFT_UP);
		m_LeftUp->CalSameDepthidx();
		m_LeftUp->BuildTreeByDepth(pd3dDevice, pd3dCommandList,limitdepth);
	}

	if (!m_RightUp) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x + HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z + HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_RightUp = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_RightUp->m_Route = this->m_Route;
		m_RightUp->m_Route.push_back(RIGHT_UP);
		m_RightUp->CalSameDepthidx();
		m_RightUp->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

	if (!m_LeftBottom) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x - HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z - HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_LeftBottom = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_LeftBottom->m_Route = this->m_Route;
		m_LeftBottom->m_Route.push_back(LEFT_BOTTOM);
		m_LeftBottom->CalSameDepthidx();
		m_LeftBottom->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

	if (!m_RightBottom) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x + HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z - HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_RightBottom = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_RightBottom->m_Route = this->m_Route;
		m_RightBottom->m_Route.push_back(RIGHT_BOTTOM);
		m_RightBottom->CalSameDepthidx();
		m_RightBottom->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
	}

}

void QuadTree::InsertStaticObject(std::vector<GameObject*> object)
{

	
	XMFLOAT3 min;
	XMFLOAT3 max;
	min = Vector3::Subtract(m_BoundingBox.Center, m_BoundingBox.Extents);
	max = Vector3::Add(m_BoundingBox.Center, m_BoundingBox.Extents);

	for (auto p : object) {
		XMFLOAT3 pos = XMFLOAT3(p->m_xmf4x4World._41, p->m_xmf4x4World._42, p->m_xmf4x4World._43);
		if (min.x <= pos.x&& pos.x < max.x && min.z <= pos.z && pos.z < max.z) {
			p->m_checkref++;
			m_StaticObject.push_back(p);
		}
		
	}

}



void QuadTree::InsertDynamicObject(){

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

void QuadTree::BringDepthTrees(std::list<QuadTree*>& out, int ndepth)
{
	static int count  = 0;

	if ((ndepth == m_depth)) {
		out.push_back(this);// 같은 
		return;
	}

	if (m_LeftBottom)
		m_LeftBottom->BringDepthTrees(out, ndepth);


	if (m_RightBottom)
		m_RightBottom->BringDepthTrees(out, ndepth);

	if (m_RightUp)
		m_RightUp->BringDepthTrees(out, ndepth);

	if (m_LeftUp)
		m_LeftUp->BringDepthTrees(out, ndepth);






}

void QuadTree::CalSameDepthidx()
{
	int depth = m_depth;
	int x = 0;
	int y = 0;
	int size = pow(2, depth-1); // 각 단계에서의 노드 크기

	for (const int& dir : m_Route) {
		switch (dir) {
		case LEFT_UP:
			y += size;
			break;
		case RIGHT_UP:
			x += size;
			y += size;
			break;
		case LEFT_BOTTOM:
			// x, y 변동 없음
			break;
		case RIGHT_BOTTOM:
			x += size;
			break;
		}
		size /= 2; // 각 단계에서 노드 크기 절반으로 줄이기
	}

	// 해당 깊이의 인덱스 계산
	int numNodesPerRow = pow(2, m_depth  );
	m_SameDepthidx = numNodesPerRow * y + x;

}

void QuadTree::AnimateObjects(float elapsedTime, vector<GameObject*>& Enemys)
{

	XMFLOAT3 min;
	XMFLOAT3 max;
	min = Vector3::Subtract(m_BoundingBox.Center, m_BoundingBox.Extents);
	max = Vector3::Add(m_BoundingBox.Center, m_BoundingBox.Extents);

	m_DynamicObject.clear();

	for (auto& p : Enemys) {
		XMFLOAT3 pos = p->GetPosition();
		if (min.x <= pos.x && pos.x < max.x && min.z <= pos.z && pos.z < max.z) {

			m_DynamicObject.push_back(p);
		}

	}

	for (auto& GO : m_DynamicObject) {

		if (GO->m_bActive) {
			((AlienSpider*)(GO))->Update(elapsedTime);

			if (((AlienSpider*)(GO))->m_GoalType != Jump_Goal)
				((AlienSpider*)(GO))->m_pPerception->IsLookPlayer(m_pPlayer);
			else if (((AlienSpider*)(GO))->m_pSoul->m_JumpStep >= JUMP_LANDING)
				((AlienSpider*)(GO))->m_pPerception->IsLookPlayer(m_pPlayer);
			((AlienSpider*)(GO))->AnimateWithMultithread(elapsedTime, m_SameDepthidx);

		}
	}

}

void QuadTree::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (auto& GO : m_StaticObject) {
		if (m_pCamera->IsInFrustum(GO->m_BoundingBox)) GO->Render(pd3dCommandList);
	}

		for (auto& GO : m_DynamicObject) {

			if (GO->m_bActive) {
				if (GO->m_bVisible) GO->Render(pd3dCommandList);
			}

		}




}


