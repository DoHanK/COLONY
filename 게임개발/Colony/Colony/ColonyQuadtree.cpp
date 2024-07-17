#include "ColonyQuadtree.h"


mutex test;

QuadTree::QuadTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int depth, XMFLOAT3 center, XMFLOAT3 Extend) :m_depth(depth), m_BoundingBox(BoundingBox(center, Extend))
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
void QuadTree::Release() {
	if (m_LeftUp) m_LeftUp->Release();
	if (m_RightUp) m_RightUp->Release();
	if (m_LeftBottom) m_LeftBottom->Release();
	if (m_RightBottom) m_RightBottom->Release();

	if (--m_nReferences <= 0) delete this;
}

bool QuadTree::BuildTreeByDepth(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int limitdepth) {

	if (limitdepth <= m_depth) {
		return false;
	}

	XMFLOAT3 Center = m_BoundingBox.Center;
	XMFLOAT3 Extend = m_BoundingBox.Extents;
	float HalfExtendx = Extend.x / 2;
	float HalfExtendy = Extend.y / 2;
	float HalfExtendz = Extend.z / 2;

	if (!m_LeftUp) {

		XMFLOAT3 LUCenter = XMFLOAT3(m_BoundingBox.Center.x - HalfExtendx, m_BoundingBox.Center.y, m_BoundingBox.Center.z + HalfExtendz);

		XMFLOAT3 LUExtend = XMFLOAT3(HalfExtendx, Extend.y, HalfExtendz);

		m_LeftUp = new QuadTree(pd3dDevice, pd3dCommandList, m_depth + 1, LUCenter, LUExtend);

		m_LeftUp->m_Route = this->m_Route;
		m_LeftUp->m_Route.push_back(LEFT_UP);
		m_LeftUp->CalSameDepthidx();
		m_LeftUp->BuildTreeByDepth(pd3dDevice, pd3dCommandList, limitdepth);
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
		if (min.x <= pos.x && pos.x < max.x && min.z <= pos.z && pos.z < max.z) {

			m_StaticObject.push_back(p);
		}

	}

}



void QuadTree::InsertDynamicObject() {

}

void QuadTree::SettingStaticBounding(CollisionManager& collisionManager)
{

	m_pBoundPlayer = (BCapsule*)collisionManager.m_pPlayer;
	
	Vector3::ScalarProduct(m_BoundingBox.Extents, 1.2f, false);


	for (auto o : collisionManager.m_StaticObjects) {

		if (m_BoundingBox.Intersects(((BOBBox*)o)->m_boundingbox)) {

			m_StaticBoundings.push_back(o);

		}
	}

}

void QuadTree::BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList, int DepthLevel) {

	if (m_BoundingMesh && (DepthLevel == m_depth)) {
		m_BoundingMesh->Render(pd3dCommandList);
	}

	//ChildNode
	if (m_LeftUp)
		m_LeftUp->BoundingRendering(pd3dCommandList, DepthLevel);

	if (m_RightUp)
		m_RightUp->BoundingRendering(pd3dCommandList, DepthLevel);

	if (m_LeftBottom)
		m_LeftBottom->BoundingRendering(pd3dCommandList, DepthLevel);

	if (m_RightBottom)
		m_RightBottom->BoundingRendering(pd3dCommandList, DepthLevel);

}

void QuadTree::BringDepthTrees(std::vector<QuadTree*>& out, int ndepth)
{
	static int count = 0;

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
	int size = pow(2, depth - 1); // 각 단계에서의 노드 크기

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
	int numNodesPerRow = pow(2, m_depth);
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

	CollisionEnemyToStaticObject();
	CollisionEnemyToPlayer();
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

void QuadTree::CollisionEnemyToStaticObject()
{

	//콜리전 위치 이동
	int count = 0;
	std::list<AliensBoudingBox*> m_RemoveObjects;



	for (auto& E : m_DynamicObject) {
		float dis = XM3CalDis(m_pCamera->GetPosition(), E->GetPosition());
		
		AliensBoudingBox AliensBound(E,E->m_MonsterScale);
		AliensBound.UpdateCollisionDetectBouding();
		AliensBound.UpdateEntireBouding();
		int inputcount = 0;
		if (m_pCamera->IsInFrustum(AliensBound.m_Entire) && dis < 150.f) AliensBound.m_pOwner->m_bVisible = true;
		//if (m_pCamera->IsInFrustum(AliensBound.m_Entire) ) AliensBound.m_pOwner->m_bVisible = true;
		else AliensBound.m_pOwner->m_bVisible = false;

		for (const auto& a : m_StaticBoundings) {
			//전체 바운딩박스 이동


				AliensBound.UpdateEntireBouding();
			if (AliensBound.m_Entire.Intersects(((BOBBox*)a)->m_boundingbox)) {
				XMFLOAT3 m_EnemyPos = AliensBound.m_Entire.Center;
	
				//충돌면 구하기
				XMFLOAT3 m_BoundingCorner[8];

				XMFLOAT3 BoxCenter = reinterpret_cast<BOBBox*>(a)->m_boundingbox.Center;
				((BOBBox*)a)->m_boundingbox.GetCorners(m_BoundingCorner);

				XMFLOAT3 SphereToBox = Vector3::Normalize(Vector3::Subtract(m_EnemyPos, BoxCenter));


				// 뒷면 , 앞면 , 오른쪽 면 , 왼쪽 면 , 아래면 , 윗면
				XMFLOAT3 PLANECENTER[6];
				XMFLOAT3 PLANENORMAL[6];


				// 앞면
				PLANECENTER[0].x = (m_BoundingCorner[0].x + m_BoundingCorner[1].x + m_BoundingCorner[2].x + m_BoundingCorner[3].x) / 4;
				PLANECENTER[0].y = (m_BoundingCorner[0].y + m_BoundingCorner[1].y + m_BoundingCorner[2].y + m_BoundingCorner[3].y) / 4;
				PLANECENTER[0].z = (m_BoundingCorner[0].z + m_BoundingCorner[1].z + m_BoundingCorner[2].z + m_BoundingCorner[3].z) / 4;

				// 뒷면
				PLANECENTER[1].x = (m_BoundingCorner[4].x + m_BoundingCorner[5].x + m_BoundingCorner[6].x + m_BoundingCorner[7].x) / 4;
				PLANECENTER[1].y = (m_BoundingCorner[4].y + m_BoundingCorner[5].y + m_BoundingCorner[6].y + m_BoundingCorner[7].y) / 4;
				PLANECENTER[1].z = (m_BoundingCorner[4].z + m_BoundingCorner[5].z + m_BoundingCorner[6].z + m_BoundingCorner[7].z) / 4;

				// 오른쪽 면
				PLANECENTER[2].x = (m_BoundingCorner[1].x + m_BoundingCorner[5].x + m_BoundingCorner[6].x + m_BoundingCorner[2].x) / 4;
				PLANECENTER[2].y = (m_BoundingCorner[1].y + m_BoundingCorner[5].y + m_BoundingCorner[6].y + m_BoundingCorner[2].y) / 4;
				PLANECENTER[2].z = (m_BoundingCorner[1].z + m_BoundingCorner[5].z + m_BoundingCorner[6].z + m_BoundingCorner[2].z) / 4;

				// 왼쪽 면
				PLANECENTER[3].x = (m_BoundingCorner[0].x + m_BoundingCorner[4].x + m_BoundingCorner[7].x + m_BoundingCorner[3].x) / 4;
				PLANECENTER[3].y = (m_BoundingCorner[0].y + m_BoundingCorner[4].y + m_BoundingCorner[7].y + m_BoundingCorner[3].y) / 4;
				PLANECENTER[3].z = (m_BoundingCorner[0].z + m_BoundingCorner[4].z + m_BoundingCorner[7].z + m_BoundingCorner[3].z) / 4;

				// 윗면
				PLANECENTER[4].x = (m_BoundingCorner[3].x + m_BoundingCorner[2].x + m_BoundingCorner[6].x + m_BoundingCorner[7].x) / 4;
				PLANECENTER[4].y = (m_BoundingCorner[3].y + m_BoundingCorner[2].y + m_BoundingCorner[6].y + m_BoundingCorner[7].y) / 4;
				PLANECENTER[4].z = (m_BoundingCorner[3].z + m_BoundingCorner[2].z + m_BoundingCorner[6].z + m_BoundingCorner[7].z) / 4;

				// 아랫면
				PLANECENTER[5].x = (m_BoundingCorner[0].x + m_BoundingCorner[1].x + m_BoundingCorner[5].x + m_BoundingCorner[4].x) / 4;
				PLANECENTER[5].y = (m_BoundingCorner[0].y + m_BoundingCorner[1].y + m_BoundingCorner[5].y + m_BoundingCorner[4].y) / 4;
				PLANECENTER[5].z = (m_BoundingCorner[0].z + m_BoundingCorner[1].z + m_BoundingCorner[5].z + m_BoundingCorner[4].z) / 4;



				for (int i = 0; i < 6; ++i) {
					PLANENORMAL[i] = Vector3::Normalize(Vector3::Subtract(PLANECENTER[i], BoxCenter));
				}

				int selectNum = -1;
				float minDistance = FLT_MAX;
				for (int i = 0; i < 6; ++i) {
					if (0 < Vector3::DotProduct(PLANENORMAL[i], SphereToBox)) {
						float distance = DistancePointToPlane(m_EnemyPos, PLANENORMAL[i], PLANECENTER[i]);
						if (distance < minDistance) {
							selectNum = i;
							minDistance = distance;
						}
					}
				}
				if ((((AlienSpider*)AliensBound.m_pOwner)->m_GoalType != Wander_Goal) &&
					(((AlienSpider*)AliensBound.m_pOwner)->m_GoalType != FollowPath_Goal)
					) {

			
					//살아있을때만 적용
					if (AliensBound.m_pOwner->m_bActive == true) {
						float dotProduct = Vector3::DotProduct(AliensBound.m_pOwner->m_xmfPre3Velocity, PLANENORMAL[selectNum]);

						if (dotProduct <= EPSILON) {
							XMFLOAT3 slidingVector = Vector3::Subtract(AliensBound.m_pOwner->m_xmfPre3Velocity, { PLANENORMAL[selectNum].x * dotProduct, PLANENORMAL[selectNum].y * dotProduct, PLANENORMAL[selectNum].z * dotProduct });
							(AliensBound.m_pOwner)->RollbackPosition();
							XMFLOAT3 TempPos = XMFLOAT3(AliensBound.m_pOwner->m_xmf4x4ToParent._41, AliensBound.m_pOwner->m_xmf4x4ToParent._42, AliensBound.m_pOwner->m_xmf4x4ToParent._43);
							(AliensBound.m_pOwner)->m_xmfPre3Position = TempPos;
							(AliensBound.m_pOwner)->m_xmfPre3Velocity = slidingVector;
							(AliensBound.m_pOwner)->AddPostion(slidingVector);

						}
					}

				}



				if (selectNum == 4) {



					if (((AlienSpider*)AliensBound.m_pOwner)->m_pSoul->m_JumpStep == JUMPING &&
						((AlienSpider*)AliensBound.m_pOwner)->m_pSoul->m_JumpStep != JUMP_LANDING &&
						((AlienSpider*)AliensBound.m_pOwner)->m_pSoul->m_JumpStep != JUMP_END &&
						AliensBound.m_pOwner->m_xmf3Velocity.y < 0) {

						((AlienSpider*)AliensBound.m_pOwner)->m_pSoul->m_JumpStep = JUMP_LANDING;

					}

					if (((AlienSpider*)AliensBound.m_pOwner)->m_GoalType != Jump_Goal) {

						if (PLANECENTER[4].y > 0.5f) {
							((AlienSpider*)AliensBound.m_pOwner)->m_pBrain->m_NeedJump = true;
						}
					}
					else {
						if (((AlienSpider*)AliensBound.m_pOwner)->m_pSoul->m_JumpStep == JUMP_LANDING) {

						}

					}
					(AliensBound.m_pOwner)->m_xmfPre3Position.y = PLANECENTER[4].y;
					if (AliensBound.m_pOwner->m_xmf3Velocity.y < 0) {
						(AliensBound.m_pOwner)->m_xmf3Velocity.y = 0;
						(AliensBound.m_pOwner)->m_xmfPre3Velocity.y = 0;

					}
					(AliensBound.m_pOwner)->m_xmf4x4ToParent._42 = PLANECENTER[4].y ;
					if (AliensBound.m_pOwner->m_bActive == false && inputcount == 0) {

					}
				}
			}
		}
	}




}

void QuadTree::CollisionEnemyToPlayer()
{
	for (auto& E : m_DynamicObject) {

		AliensBoudingBox AliensBound(E, E->m_MonsterScale);
		AliensBound.UpdateCollisionDetectBouding();
		if (AliensBound.m_pOwner->m_HP > 0) {
			AliensBound.UpdateEntireBouding();
			BoundingSphere boundingsphere = ((BCapsule*)m_pBoundPlayer)->GetCapsuleBounding(AliensBound.m_Entire.Center);
			if (boundingsphere.Intersects(AliensBound.m_Entire)) {



				XMFLOAT3 BackDir = Vector3::Subtract(AliensBound.m_Entire.Center, boundingsphere.Center);
				float CapDis = boundingsphere.Radius + AliensBound.m_Entire.Radius;
				float RealDis = XM3CalDis(boundingsphere.Center, AliensBound.m_Entire.Center);
				float MoveDis = CapDis - RealDis;

				BackDir = Vector3::ScalarProduct(BackDir, MoveDis, true);
				BackDir.x *= 5.0f;
				BackDir.z *= 5.0f;
				AliensBound.m_pOwner->AddPostion(BackDir);
			}
		}
	}
}


