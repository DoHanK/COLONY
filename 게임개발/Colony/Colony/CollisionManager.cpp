#include "CollisionManager.h"
#include "AlienSpider.h"


class AlienSpider;
// 벡터의 길이(크기)를 계산하는 함수


CollisionManager::CollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,SoundManager* soundmanager)
{
	for (int i = 0; i < 3000; ++i) {
		BoundingBoxMesh* pBounding = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
		m_BoundingBoxMeshes.push_back(pBounding);
	}

	for (int i = 0; i < 500; ++i) {
		BoundingBoxMesh* pBounding = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
		m_SubBoundingBoxMeshes.push_back(pBounding);

	}

	 m_pCapsuleMesh = new CapsuleMesh(pd3dDevice, pd3dCommandList,20,10,1,1,0);
	m_pPlayerCapsuleMesh = new CapsuleMesh(pd3dDevice, pd3dCommandList,20,10, 0.3, 1.3, 0.3);
	m_psphere = new ShphereMesh(pd3dDevice, pd3dCommandList, 20, 10,1) ;

	//사운드 로드
	m_pSoundManager = soundmanager;
	HittedSound = m_pSoundManager->LoadWaveToBuffer("Sound/Click_Electronic_04.wav");
	HittedSound->SetVolume(-300);
	ShortExplosion = m_pSoundManager->LoadWaveToBuffer("Sound/shortExplosion.wav");
	ShortExplosion->SetVolume(-300);
	MonsterDie = m_pSoundManager->LoadWaveToBuffer("Sound/MonsterDie.wav");
	MonsterDie->SetVolume(-300);
	MonsterDieEffect = m_pSoundManager->LoadWaveToBuffer("Sound/monsterDieEffect.wav");
	MonsterDieEffect->SetVolume(0);

}

CollisionManager::~CollisionManager()
{

	for (auto& SM : m_StaticObjects) {
		delete SM;
	}

	for (auto& SM : m_SubStaticObjects) {
		delete SM;
	}

	for (auto& SM : m_AccelationObjects) {
		delete SM;
	}

	for (auto& SM : m_EnemyObjects) {
		delete SM;
	}

	for (auto& BoxMesh : m_BoundingBoxMeshes) {
		BoxMesh->Release();
	}

	for (auto& BoxMesh : m_SubBoundingBoxMeshes) {
		BoxMesh->Release();
	}

	if(m_pCapsuleMesh) m_pCapsuleMesh->Release();
	if(m_pPlayerCapsuleMesh) m_pPlayerCapsuleMesh->Release();
	if(m_psphere) m_psphere->Release();
}
void CollisionManager::ReleaseUploadBuffers()
{

	if (m_pCapsuleMesh) m_pCapsuleMesh->ReleaseUploadBuffers();
	if (m_pPlayerCapsuleMesh) m_pPlayerCapsuleMesh->ReleaseUploadBuffers();
	if (m_psphere) m_psphere->ReleaseUploadBuffers();
}
void CollisionManager::SetHealItem(Item* pGameObject,const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(health);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/Health.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(2.0f, 2.0f, 2.0f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));

}
void CollisionManager::SetSpeedItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(speed);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/speed.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(2.0f, 2.0f, 2.0f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));

}
void CollisionManager::SetDamageItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(damage);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/Damage.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(2.0f, 2.0f, 2.0f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));
}
void CollisionManager::SetSheildItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(sheild);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/Shield.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(2.0f, 2.0f, 2.0f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));

}
XMFLOAT4X4 CollisionManager::MakeScaleMatrix(float x, float y, float z)
{

	XMFLOAT4X4 m = Matrix4x4::Identity();

	m._11 *= x;
	m._22 *= y;
	m._33 *= z;

	return m;
}

XMFLOAT4X4 CollisionManager::GetSphereMatrix(BSphere* pSphere)
{
	float r = pSphere->m_radius;
	XMFLOAT4X4 SM = MakeScaleMatrix(r, r, r);
	XMFLOAT4X4 WM = Matrix4x4::Identity();
	if (pSphere->m_pOwner) WM = pSphere->m_pOwner->m_xmf4x4World;
	return Matrix4x4::Multiply(SM, WM);
}

XMFLOAT4X4 CollisionManager::GetSphereMatrix(const float& r , const XMFLOAT3& center)
{
	XMFLOAT4X4 SM = MakeScaleMatrix(r, r, r);
	XMFLOAT4X4 WM = Matrix4x4::Identity();
	WM._41 = center.x;
	WM._42 = center.y;
	WM._43 = center.z;
	return Matrix4x4::Multiply(SM, WM);
}

XMFLOAT4X4 CollisionManager::GetSphereMatrix(BCapsule* pCapsule)
{
	float r = pCapsule->m_radius;
	XMFLOAT4X4 SM = MakeScaleMatrix(r, r, r);
	XMFLOAT4X4 WM = Matrix4x4::Identity();
	if (pCapsule) {
		WM._41 = pCapsule->m_boundingshpere.Center.x;
		WM._42 = pCapsule->m_boundingshpere.Center.y;
		WM._43 = pCapsule->m_boundingshpere.Center.z;
	}
	return Matrix4x4::Multiply(SM, WM);
}


//박스 등록
void CollisionManager::EnrollObjectIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, GameObject* pOwner)
{
	BOBBox* pBox = new BOBBox(center, extend, pOwner);

	if (isAccel) {
		m_AccelationObjects.push_back(pBox);

	}
	else {
		m_StaticObjects.push_back(pBox);
	}

}
//행렬을 이용해서 바운딩 박스 크기
void CollisionManager::EnrollObjectIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend,XMFLOAT4X4 Transform, GameObject* pOwner)
{
	XMFLOAT3 EXTEND = extend;
	EXTEND.x *= Transform._11;
	EXTEND.y *= Transform._22;
	EXTEND.z *= Transform._33;

	BOBBox* pBox = new BOBBox(center, EXTEND, pOwner);

	if (isAccel) {
		m_AccelationObjects.push_back(pBox);

	}
	else {

		m_StaticObjects.push_back(pBox);

	}

}
// 휴리스틱하게 바운딩 박스를 맞출때 사용..
void CollisionManager::EnrollObjectIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT3 extendscale, GameObject* pOwner)
{
	XMFLOAT3 EXTEND = extend;
	EXTEND.x *= extendscale.x;
	EXTEND.y *= extendscale.y;
	EXTEND.z *= extendscale.z;


	BOBBox* pBox = new BOBBox(center, extend, pOwner);
	if (isAccel) {
		m_AccelationObjects.push_back(pBox);

	}
	else {
		m_StaticObjects.push_back(pBox);
	}

}

void CollisionManager::EnrollItemIntoBox( XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT4X4 Transform, GameObject* pOwner)
{



	BOBBox* pBox = new BOBBox(center, extend , pOwner);
	pBox->m_boundingbox.Transform(pBox->m_boundingbox, XMLoadFloat4x4(&Transform));

	m_ItemBoxes.push_back(pBox);
	
}

void CollisionManager::EnrollHierarchicalStaticGameObject(GameObject* pOwner)
{
	vector<GameObject*> Frameboxs;
	pOwner->FindAllFrame(Frameboxs);


	for (auto Frame : Frameboxs) {
		if (Frame->m_pMesh) {
			
			XMFLOAT3 extend(Frame->m_pMesh->GetAABBExtend());

			BOBBox* pBox = new BOBBox(Frame->m_pMesh->GetAABBCenter(), extend, Frame);

				pBox->UpdateCollision();
				pBox->m_boundingbox = pBox->m_Transformboudingbox;
				m_SubStaticObjects.push_back(pBox);
				m_SubBoundingBoxMeshes[subboundingcur++]->UpdateVertexPosition(&pBox->m_Transformboudingbox);
			

		}

	}

}

//구 등록
void CollisionManager::EnrollObjectIntoSphere(bool isAccel, XMFLOAT3 center, float radius, GameObject* pOwner)
{
	BSphere* psphere = new BSphere(center, radius, pOwner);

	if (isAccel) {
		m_AccelationObjects.push_back(psphere);
	}
	else {
		m_StaticObjects.push_back(psphere);
	}

}
//캡슐 등록
void CollisionManager::EnrollObjectIntoCapsule(bool isAccel, XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner)
{
	BCapsule* pCapsule = new BCapsule(center,radius,tip ,base, pOwner);

	if (isAccel) {
		m_AccelationObjects.push_back(pCapsule);
	}
	else {
		m_StaticObjects.push_back(pCapsule);
	}

}
//캡슐 플레이어 등록
void CollisionManager::EnrollPlayerIntoCapsule(XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner)
{

	BCapsule* pCapsule = new BCapsule(center, radius, tip, base, pOwner);

	m_pPlayer = pCapsule;
}
// 총알 등록
void CollisionManager::EnrollbulletIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT4X4 Transform, GameObject* pOwner)
{
	XMFLOAT3 EXTEND = extend;
	EXTEND.x *= Transform._11;
	EXTEND.y *= Transform._22;
	EXTEND.z *= Transform._33;

	BOBBox* pBox = new BOBBox(center, EXTEND, pOwner);

	m_bullets.push_back(pBox);

	
}
// 바운디 정보 불러오기
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
		BOBBox* bxinfo = new BOBBox(XMFLOAT3(0, 0, 0), Extend, NULL);
		bxinfo->mxf4x4Position = mxf4x4Position;
		bxinfo->m_boundingbox.Transform(bxinfo->m_boundingbox, DirectX::XMLoadFloat4x4(&mxf4x4Position));
		bxinfo->m_Transformboudingbox = bxinfo->m_boundingbox;
		m_StaticObjects.push_back(bxinfo);

		

	
	}
	

	is.close();
}


void CollisionManager::EnrollEnemy(GameObject* pEnemy)
{
	if (pEnemy) {
		
		AliensBoudingBox* m_pBounding = new AliensBoudingBox(pEnemy, ((AlienSpider*)pEnemy)->m_MonsterScale);

		m_EnemyObjects.emplace_back(m_pBounding);

	}
}

void CollisionManager::EnrollDogEnemy(GameObject* pEnemy)
{
	if(pEnemy) {

		BSphere* pDogEnemy = new BSphere(XMFLOAT3(0,0.3,0),0.6f, pEnemy);

		m_DogEnemy.emplace_back(pDogEnemy);

	}
}

void CollisionManager::CheckCollisionEnemytoStaticObject(GameObject* pEnemy)
{


	
		float dis = XM3CalDis(m_pCamera->GetPosition(),pEnemy->GetPosition());
		AliensBoudingBox AliensBound(pEnemy, pEnemy->m_MonsterScale);
		AliensBound.UpdateCollisionDetectBouding();
		int inputcount = 0;
		AliensBound.UpdateEntireBouding();
		if (m_pCamera->IsInFrustum(AliensBound.m_Entire) && dis < 100.f) AliensBound.m_pOwner->m_bVisible = true;
		else AliensBound.m_pOwner->m_bVisible = false;

		for (const auto& a : m_StaticObjects) {
			//전체 바운딩박스 이동


			if (AliensBound.m_Obstable.Intersects(((BOBBox*)a)->m_boundingbox)) {
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
						float distance = DistancePointToPlane(SphereToBox, PLANENORMAL[i], PLANECENTER[i]);
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
					(AliensBound.m_pOwner)->m_xmf4x4ToParent._42 = PLANECENTER[4].y;
					if (AliensBound.m_pOwner->m_bActive == false && inputcount == 0) {

					}
				}
			}
		}
	

}

void CollisionManager::EnrollRedZoneIntoSphere(XMFLOAT3 center, float radius, GameObject* pOwner)
{
	BSphere* psphere = new BSphere(center, radius, pOwner);

	m_pRedZoneCollision = psphere;

}

bool CollisionManager::CollisionPlayerToStaticObeject()
{

	
	//콜리전 위치 이동
	int count = 0;
	for (const auto &a: m_StaticObjects) {
		BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(*reinterpret_cast<BOBBox*>(a));
		if (boundingsphere.Intersects(((BOBBox*)a)->m_boundingbox)) {
			m_PlayerCapsulePos = boundingsphere.Center;
			((BCapsule*)m_pPlayer)->m_boundingshpere.Center = m_PlayerCapsulePos;
			
			

			//충돌면 구하기
			XMFLOAT3 m_BoundingCorner[8];
			
			XMFLOAT3 BoxCenter = reinterpret_cast<BOBBox*>(a)->m_boundingbox.Center;
			((BOBBox*)a)->m_boundingbox.GetCorners(m_BoundingCorner);
			//캡슐의 센터
			XMFLOAT3 CapsuleCenter = m_PlayerCapsulePos;
			XMFLOAT3 CapsuleToBox = Vector3::Normalize(Vector3::Subtract(CapsuleCenter, BoxCenter));


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
				if (0 < Vector3::DotProduct(PLANENORMAL[i], CapsuleToBox)) {
					float distance = DistancePointToPlane(CapsuleCenter, PLANENORMAL[i], PLANECENTER[i]);
					if (distance < minDistance) {
						selectNum = i;
						minDistance = distance;
					}
				}
			}
			// selectNum 0 뒷 , 1 앞 , 2 오른 ,3 왼 ,4 위 ,5  아래

			//string tt = "";
			//switch (selectNum) {
			//case 0:
			//	tt = "뒷 ";
			//	break;
			//case 1:
			//	tt = "앞 ";
			//	break;
			//case 2:
			//	tt = "오른 ";
			//	break;
			//case 3:
			//	tt = "왼 ";
			//	break;
			//case 4:
			//	tt = "위 ";
			//	break;
			//case 5:
			//	tt = "아래 ";
			//	break;
			//default:
			//	tt = " 충돌면 없음 ";
			//	break;

			//}

			//count++;
			//string temp = "충돌  ";
			//temp += to_string(count);
			//temp += "번호  ";
			////temp += to_string(selectNum);
			//temp += tt;
			//temp += "\n";
			//OutputDebugStringA(temp.c_str());


			float dotProduct = Vector3::DotProduct(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, PLANENORMAL[selectNum]);
			if (dotProduct <= EPSILON) {
				XMFLOAT3 slidingVector = Vector3::Subtract(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, { PLANENORMAL[selectNum].x * dotProduct, PLANENORMAL[selectNum].y * dotProduct, PLANENORMAL[selectNum].z * dotProduct });
				((Player*)m_pPlayer->m_pOwner)->RollbackPosition();
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position = ((Player*)m_pPlayer->m_pOwner)->m_xmf3Position;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity = slidingVector;
				((Player*)m_pPlayer->m_pOwner)->AddPosition(slidingVector);
	//			if (a->m_pOwner) {
	//				OutputDebugStringA(a->m_pOwner->m_pstrFrameName);
	//				;
	//							string temp = "x  ";
	//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.x);
	// temp += "y  ";
	//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.y);
	// temp += "z  ";
	//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.z);
	//OutputDebugStringA(temp.c_str());
	//				OutputDebugStringA("충돌\n");
	//				
	//			}
	//			string temp = "x  ";
	//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.x);
	// temp += "y  ";
	//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y);
	// temp += "z  ";
	//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.z);
	//temp += "\n";
	//OutputDebugStringA(temp.c_str());
			}

			if (selectNum == 4) {
				((Player*)m_pPlayer->m_pOwner)->isJump = false;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position.y = PLANECENTER[4].y;
				if (((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y < 0) {
					((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y = 0;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y = 0;
				}
				//((Player*)m_pPlayer->m_pOwner)->m_xmf3Position.y = PLANECENTER[4].y;
			}
		}

	}


	
	return true;
}


bool CollisionManager::CollisionPlayerToSubSceneStaticObeject()
{


	//콜리전 위치 이동
	int count = 0;
	for (const auto& a : m_SubStaticObjects) {
		BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(*reinterpret_cast<BOBBox*>(a));
		if (boundingsphere.Intersects(((BOBBox*)a)->m_boundingbox)) {
			m_PlayerCapsulePos = boundingsphere.Center;
			((BCapsule*)m_pPlayer)->m_boundingshpere.Center = m_PlayerCapsulePos;



			//충돌면 구하기
			XMFLOAT3 m_BoundingCorner[8];

			XMFLOAT3 BoxCenter = reinterpret_cast<BOBBox*>(a)->m_boundingbox.Center;
			((BOBBox*)a)->m_boundingbox.GetCorners(m_BoundingCorner);
			//캡슐의 센터
			XMFLOAT3 CapsuleCenter = m_PlayerCapsulePos;
			XMFLOAT3 CapsuleToBox = Vector3::Normalize(Vector3::Subtract(CapsuleCenter, BoxCenter));


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
				if (0 < Vector3::DotProduct(PLANENORMAL[i], CapsuleToBox)) {
					float distance = DistancePointToPlane(CapsuleCenter, PLANENORMAL[i], PLANECENTER[i]);
					if (distance < minDistance) {
						selectNum = i;
						minDistance = distance;
					}
				}
			}
			// selectNum 0 뒷 , 1 앞 , 2 오른 ,3 왼 ,4 위 ,5  아래

			//string tt = "";
			//switch (selectNum) {
			//case 0:
			//	tt = "뒷 ";
			//	break;
			//case 1:
			//	tt = "앞 ";
			//	break;
			//case 2:
			//	tt = "오른 ";
			//	break;
			//case 3:
			//	tt = "왼 ";
			//	break;
			//case 4:
			//	tt = "위 ";
			//	break;
			//case 5:
			//	tt = "아래 ";
			//	break;
			//default:
			//	tt = " 충돌면 없음 ";
			//	break;

			//}

			//count++;
			//string temp = "충돌  ";
			//temp += to_string(count);
			//temp += "번호  ";
			////temp += to_string(selectNum);
			//temp += tt;
			//temp += "\n";
			//OutputDebugStringA(temp.c_str());


			float dotProduct = Vector3::DotProduct(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, PLANENORMAL[selectNum]);
			if (dotProduct <= EPSILON) {
				XMFLOAT3 slidingVector = Vector3::Subtract(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, { PLANENORMAL[selectNum].x * dotProduct, PLANENORMAL[selectNum].y * dotProduct, PLANENORMAL[selectNum].z * dotProduct });
				((Player*)m_pPlayer->m_pOwner)->RollbackPosition();
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position = ((Player*)m_pPlayer->m_pOwner)->m_xmf3Position;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity = slidingVector;
				((Player*)m_pPlayer->m_pOwner)->AddPosition(slidingVector);
				//			if (a->m_pOwner) {
				//				OutputDebugStringA(a->m_pOwner->m_pstrFrameName);
				//				;
				//							string temp = "x  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.x);
				// temp += "y  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.y);
				// temp += "z  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.z);
				//OutputDebugStringA(temp.c_str());
				//				OutputDebugStringA("충돌\n");
				//				
				//			}
				//			string temp = "x  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.x);
				// temp += "y  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y);
				// temp += "z  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.z);
				//temp += "\n";
				//OutputDebugStringA(temp.c_str());
			}

			if (selectNum == 4) {
				((Player*)m_pPlayer->m_pOwner)->isJump = false;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position.y = PLANECENTER[4].y;
				if (((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y < 0) {
					((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y = 0;
					((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y = 0;
				}
				//((Player*)m_pPlayer->m_pOwner)->m_xmf3Position.y = PLANECENTER[4].y;
			}
		}

	}



	return true;
}

bool CollisionManager::CollisionPlayerToItemBox()
{
	//콜리전 위치 이동
	int count = 0;
	for (const auto& a : m_ItemBoxes) {
		BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(*reinterpret_cast<BOBBox*>(a));
		if (boundingsphere.Intersects(((BOBBox*)a)->m_Transformboudingbox)) {
			m_PlayerCapsulePos = boundingsphere.Center;
			((BCapsule*)m_pPlayer)->m_boundingshpere.Center = m_PlayerCapsulePos;



			//충돌면 구하기
			XMFLOAT3 m_BoundingCorner[8];

			XMFLOAT3 BoxCenter = reinterpret_cast<BOBBox*>(a)->m_Transformboudingbox.Center;
			((BOBBox*)a)->m_Transformboudingbox.GetCorners(m_BoundingCorner);
			//캡슐의 센터
			XMFLOAT3 CapsuleCenter = m_PlayerCapsulePos;
			XMFLOAT3 CapsuleToBox = Vector3::Normalize(Vector3::Subtract(CapsuleCenter, BoxCenter));


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
				if (0 < Vector3::DotProduct(PLANENORMAL[i], CapsuleToBox)) {
					float distance = DistancePointToPlane(CapsuleCenter, PLANENORMAL[i], PLANECENTER[i]);
					if (distance < minDistance) {
						selectNum = i;
						minDistance = distance;
					}
				}
			}
			// selectNum 0 뒷 , 1 앞 , 2 오른 ,3 왼 ,4 위 ,5  아래

			//string tt = "";
			//switch (selectNum) {
			//case 0:
			//	tt = "뒷 ";
			//	break;
			//case 1:
			//	tt = "앞 ";
			//	break;
			//case 2:
			//	tt = "오른 ";
			//	break;
			//case 3:
			//	tt = "왼 ";
			//	break;
			//case 4:
			//	tt = "위 ";
			//	break;
			//case 5:
			//	tt = "아래 ";
			//	break;
			//default:
			//	tt = " 충돌면 없음 ";
			//	break;

			//}

			//count++;
			//string temp = "충돌  ";
			//temp += to_string(count);
			//temp += "번호  ";
			////temp += to_string(selectNum);
			//temp += tt;
			//temp += "\n";
			//OutputDebugStringA(temp.c_str());


			float dotProduct = Vector3::DotProduct(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, PLANENORMAL[selectNum]);
			if (dotProduct <= EPSILON) {
				XMFLOAT3 slidingVector = Vector3::Subtract(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity, { PLANENORMAL[selectNum].x * dotProduct, PLANENORMAL[selectNum].y * dotProduct, PLANENORMAL[selectNum].z * dotProduct });
				((Player*)m_pPlayer->m_pOwner)->RollbackPosition();
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position = ((Player*)m_pPlayer->m_pOwner)->m_xmf3Position;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity = slidingVector;
				((Player*)m_pPlayer->m_pOwner)->AddPosition(slidingVector);
				//			if (a->m_pOwner) {
				//				OutputDebugStringA(a->m_pOwner->m_pstrFrameName);
				//				;
				//							string temp = "x  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.x);
				// temp += "y  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.y);
				// temp += "z  ";
				//temp += to_string(((BOBBox*)a)->m_boundingbox.Center.z);
				//OutputDebugStringA(temp.c_str());
				//				OutputDebugStringA("충돌\n");
				//				
				//			}
				//			string temp = "x  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.x);
				// temp += "y  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y);
				// temp += "z  ";
				//temp += to_string(((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.z);
				//temp += "\n";
				//OutputDebugStringA(temp.c_str());
			}

			if (selectNum == 4) {
				((Player*)m_pPlayer->m_pOwner)->isJump = false;
				((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Position.y = PLANECENTER[4].y;
				if (((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y < 0) {
					((Player*)m_pPlayer->m_pOwner)->m_xmf3Velocity.y = 0;
					((Player*)m_pPlayer->m_pOwner)->m_xmfPre3Velocity.y = 0;
				}
				//((Player*)m_pPlayer->m_pOwner)->m_xmf3Position.y = PLANECENTER[4].y;
			}
		}

	}


	return true;
}

void CollisionManager::CollisionEnemyToStaticObeject() {

	//콜리전 위치 이동
	int count = 0;
	std::list<AliensBoudingBox*> m_RemoveObjects;
	for (auto& E : m_EnemyObjects) {

		int inputcount = 0;
	for (const auto& a : m_StaticObjects) {
			//전체 바운딩박스 이동
		E->UpdateCollisionDetectBouding();
			
			if (E->m_Obstable.Intersects(((BOBBox*)a)->m_boundingbox)) {
				XMFLOAT3 m_EnemyPos = E->m_Entire.Center;

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
						float distance = DistancePointToPlane(SphereToBox, PLANENORMAL[i], PLANECENTER[i]);
						if (distance < minDistance) {
							selectNum = i;
							minDistance = distance;
						}
					}
				}

				if ((((AlienSpider*)E->m_pOwner)->m_GoalType != Wander_Goal)&& 
					(((AlienSpider*)E->m_pOwner)->m_GoalType != FollowPath_Goal) 
					) {
					//살아있을때만 적용
					if (E->m_pOwner->m_bActive == true) {
						float dotProduct = Vector3::DotProduct(E->m_pOwner->m_xmfPre3Velocity, PLANENORMAL[selectNum]);
						if (dotProduct <= EPSILON) {
							XMFLOAT3 slidingVector = Vector3::Subtract(E->m_pOwner->m_xmfPre3Velocity, { PLANENORMAL[selectNum].x * dotProduct, PLANENORMAL[selectNum].y * dotProduct, PLANENORMAL[selectNum].z * dotProduct });
							(E->m_pOwner)->RollbackPosition();
							XMFLOAT3 TempPos = XMFLOAT3(E->m_pOwner->m_xmf4x4ToParent._41, E->m_pOwner->m_xmf4x4ToParent._42, E->m_pOwner->m_xmf4x4ToParent._43);
							(E->m_pOwner)->m_xmfPre3Position = TempPos;
							(E->m_pOwner)->m_xmfPre3Velocity = slidingVector;
							(E->m_pOwner)->AddPostion(slidingVector);

						}
					}

				}

				if (selectNum == 4) {

				
				
					if (((AlienSpider*)E->m_pOwner)->m_pSoul->m_JumpStep == JUMPING &&
					((AlienSpider*)E->m_pOwner)->m_pSoul->m_JumpStep != JUMP_LANDING &&
					((AlienSpider*)E->m_pOwner)->m_pSoul->m_JumpStep != JUMP_END &&
						 E->m_pOwner->m_xmf3Velocity.y < 0) {

						((AlienSpider*)E->m_pOwner)->m_pSoul->m_JumpStep = JUMP_LANDING;

					}

					if (((AlienSpider*)E->m_pOwner)->m_GoalType != Jump_Goal) {
	
						if (PLANECENTER[4].y > 0.5f) {
							((AlienSpider*)E->m_pOwner)->m_pBrain->m_NeedJump = true;
						}
					}
					else {
						if (((AlienSpider*)E->m_pOwner)->m_pSoul->m_JumpStep == JUMP_LANDING) {

						}
							
					}
					(E->m_pOwner)->m_xmfPre3Position.y = PLANECENTER[4].y;
					if (E->m_pOwner->m_xmf3Velocity.y < 0) {
						(E->m_pOwner)->m_xmf3Velocity.y = 0;
						(E->m_pOwner)->m_xmfPre3Velocity.y = 0;

					}
					(E->m_pOwner)->m_xmf4x4ToParent._42 = PLANECENTER[4].y;
					if (E->m_pOwner->m_bActive == false && inputcount == 0 ) {
						inputcount++;
						m_RemoveObjects.push_back(E);
					}
				}
			}
		}
	}

	for (auto RE : m_RemoveObjects) {

		m_EnemyObjects.remove(RE);
		delete RE;
	}

}

void CollisionManager::CollisionPlayerToEnemy() {

	for (const auto& a : m_EnemyObjects) {
		if (a->m_pOwner->m_HP > 0) {
			a->UpdateEntireBouding();
			BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(a->m_Entire.Center);
			if (boundingsphere.Intersects(a->m_Entire)) {

				((Player*)m_pPlayer->m_pOwner)->isJump = false;
				((Player*)m_pPlayer->m_pOwner)->m_xmf3Position = ((Player*)m_pPlayer->m_pOwner)->m_xmf3FinalPosition;
			}
		}
	}
}

void CollisionManager::CollisionEnemyToPlayer() { //싱글쓰레드일때 
	for (const auto& a : m_EnemyObjects) {
		if (a->m_pOwner->m_HP > 0) {
			a->UpdateEntireBouding();
			BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(a->m_Entire.Center);
			if (boundingsphere.Intersects(a->m_Entire)) {



				XMFLOAT3 BackDir = Vector3::Subtract(a->m_Entire.Center, boundingsphere.Center);
				float CapDis = boundingsphere.Radius + a->m_Entire.Radius;
				float RealDis = XM3CalDis(boundingsphere.Center, a->m_Entire.Center);
				float MoveDis = CapDis - RealDis;

				BackDir = Vector3::ScalarProduct(BackDir, MoveDis, true);
				BackDir.x *= 5.0f;
				BackDir.z *= 5.0f;
				a->m_pOwner->AddPostion(BackDir);
			}
		}
	}
}

bool CollisionManager::CollisionPlayerToRedZone()
{
	m_pRedZoneCollision->UpdateCollision();
	BoundingSphere boundingsphere = ((BCapsule*)m_pPlayer)->GetCapsuleBounding(m_pRedZoneCollision->m_center);

	if (boundingsphere.Intersects(m_pRedZoneCollision->m_boundingshpere)) {
		return true;
	}
	return false;
}

void CollisionManager::CollisionBulletToObject()
{
	//콜리전 위치 이동
	int count = 0;
	for (auto& b : m_bullets) {

		if (b->m_pOwner->m_bActive == true && ((BulletCasing*)b->m_pOwner)->m_bcrushed == false) {
			((BOBBox*)b)->UpdateCollision();


		

				if (b->m_pOwner->m_xmf4x4ToParent._42 < 0.1f) {
					b->m_pOwner->m_xmf4x4ToParent._42 = 0.1f;
					((BulletCasing*)b->m_pOwner)->m_bcrushed = true;
					((BulletCasing*)b->m_pOwner)->m_postCollisionSurvivalTime = 0.0f;

				}
			
		}
	}

}


void CollisionManager::CheckVisiableEnemy()
{
	for (auto& a : m_EnemyObjects) {
		a->UpdateEntireBouding();
		if (m_pCamera->IsInFrustum(a->m_Entire)) a->m_pOwner->m_bVisible = true;
		else a->m_pOwner->m_bVisible = false;
	}
}

bool CollisionManager::CollsionBulletToEnemy(vector<Billboard*>* m_pBloodBillboard,int& KillCount)
{

	FXMVECTOR BulletPos = XMLoadFloat3(&m_pCamera->GetPosition());
	
	FXMVECTOR BulletDir = XMLoadFloat3(&m_pCamera->GetLookVector());
	 

	for (auto& a : m_EnemyObjects) {
		a->UpdateEntireBouding();
		a->UpdateBodyBouding();
		a->UPdateLegBounding();
	}

	std::list<AliensBoudingBox*> m_RemoveObjects;



	float dis = 0;
	bool crush = false;

	//1차 충돌 처리
	std::list<pair<AliensBoudingBox*, float>> crushlist;
	for (AliensBoudingBox* enemy : m_EnemyObjects) {
		if (((AlienSpider*)enemy->m_pOwner)->m_GoalType != Deaded_Goal) {
			if (enemy->m_Entire.Intersects(BulletPos, BulletDir, dis)) {

				crushlist.emplace_back(enemy, dis);
				HittedSound->Play(0, 0, 0);
			}
		}
	}

	crushlist.sort([](pair<AliensBoudingBox*, float>& a, pair<AliensBoudingBox*, float>& b) {
		return a.first > b.first;});


	for (auto& enemy : crushlist) {

		if (crush == true)
			break;

		for (int i = 0; i < 6; i++) {
			if (crush == true)
				break;

			if (enemy.first->m_Bodys[i].Intersects(BulletPos, BulletDir, dis)) {
				
				for (int i = 0; i < 5; ++i) {
					 
					 int idx = EffectIndex[rand() % 11];
					for (auto& paticle : m_pBloodBillboard[idx]) {
						if (!paticle->active) {
							paticle->m_BillMesh->UpdataVertexPosition(UIRect(2.0* enemy.first ->m_pOwner->m_MonsterScale, -2.0 * enemy.first->m_pOwner->m_MonsterScale, -2.0 * enemy.first->m_pOwner->m_MonsterScale, 2.0 * enemy.first->m_pOwner->m_MonsterScale), 0.0f);
							paticle->active = true;
							//paticle->m_ownerObject = enemy.first->m_pOwner;
							paticle->m_CrashObject = &enemy.first->m_Bodys[i];
							break;
						}
					}
				}
				enemy.first->m_pOwner->m_bHitted = true;
				enemy.first->m_pOwner->m_HP -= ((Player*)m_pPlayer->m_pOwner)->GetBulletDamage() / enemy.first->m_pOwner->m_MonsterScale;
				if (enemy.first->m_pOwner->m_HP<= 0) {
					m_RemoveObjects.push_back(enemy.first);
					++KillCount;
				}
				crush = true;

				break;
			}
		}

		for (int i = 0; i < 8; i++) {
			if (crush == true)
				break;
			if (enemy.first->m_legs[i].Intersects(BulletPos, BulletDir, dis)) {

				for (int i = 0; i < 5; ++i) {
					//int idx = rand() % 15;
					int idx = EffectIndex[rand() % 11];
					for (auto& paticle : m_pBloodBillboard[idx]) {
						if (!paticle->active) {
							paticle->m_BillMesh->UpdataVertexPosition(UIRect(2.0 * enemy.first->m_pOwner->m_MonsterScale, -2.0 * enemy.first->m_pOwner->m_MonsterScale, -2.0 * enemy.first->m_pOwner->m_MonsterScale, 2.0 * enemy.first->m_pOwner->m_MonsterScale), 0.0f);
							paticle->active = true;
							paticle->m_CrashObject = &enemy.first->m_legs[i];
							break;
						}
					}
				}
				enemy.first->m_pOwner->m_bHitted = true;
				enemy.first->m_pOwner->m_HP -= ((Player*)m_pPlayer->m_pOwner)->GetBulletDamage()/ enemy.first->m_pOwner->m_MonsterScale;
				if (enemy.first->m_pOwner->m_HP <= 0) {
					m_RemoveObjects.push_back(enemy.first);
					++KillCount;
				}
				crush = true;

				break;
			}
		}

	}




	for (auto RE : m_RemoveObjects) {

		m_EnemyObjects.remove(RE);
		//MonsterDieEffect->Play(0, 0, 0);
		MonsterDie->Play(0, 0, 0);
		delete RE;
	}

	return crush;
}

bool CollisionManager::CollsionBulletToDogEnemy(vector<Billboard*>& m_pBloodBillboard, int& KillCount)
{

	FXMVECTOR BulletPos = XMLoadFloat3(&m_pCamera->GetPosition());

	FXMVECTOR BulletDir = XMLoadFloat3(&m_pCamera->GetLookVector());


	for (auto& a : m_DogEnemy) {
		a->UpdateCollision();
	}

	std::list<BSphere*> m_RemoveObjects;



	float dis = 0;
	bool crush = false;
	//1차 충돌 처리
	std::list<pair<BSphere*, float>> crushlist;
	for (auto enemy : m_DogEnemy) {
		if ((enemy->m_pOwner)->m_GoalType != Deaded_Goal) {
			if (enemy->m_boundingshpere.Intersects(BulletPos, BulletDir, dis)) {

				crushlist.emplace_back(enemy, dis);
				HittedSound->Play(0, 0, 0);
			}
		}
	}

	crushlist.sort([](pair<BSphere*, float>& a, pair<BSphere*, float>& b) {
		return a.first > b.first; });


	if (crushlist.size() > 0) {
		auto enemy = crushlist.begin();
	

		

		
		enemy->first->m_pOwner->m_bHitted = true;
		enemy->first->m_pOwner->m_HP -= ((Player*)m_pPlayer->m_pOwner)->GetBulletDamage() / enemy->first->m_pOwner->m_MonsterScale;
		if (enemy->first->m_pOwner->m_HP <= 0) {

			float scale = enemy->first->m_pOwner->m_MonsterScale;
			for (auto Effect : m_pBloodBillboard) {

				if (Effect->active == false) {
					Effect->active = true;
					Effect->m_OffsetPos = XMFLOAT3(0, 0, 0);
					Effect->m_BillMesh->UpdataVertexPosition(UIRect(1.4 * scale, 0, -0.7 * scale, 0.7 * scale), 1.0f);
					Effect->SetPosition(enemy->first->m_pOwner->GetPosition());
					XMFLOAT3 pos = enemy->first->m_pOwner->GetPosition();
					pos.y = 0.0f;
					Effect->m_StaticPos = pos;
					break;
				}
			}

			enemy->first->m_pOwner->m_bActive = false;
			m_RemoveObjects.push_back(enemy->first);
			++KillCount;
		}
		crush = true;

	}

	for (auto RE : m_RemoveObjects) {

		m_DogEnemy.remove(RE);
		//MonsterDieEffect->Play(0, 0, 0);
		MonsterDie->Play(0, 0, 0);
		delete RE;
	}

	return crush;
}


bool CollisionManager::CollsionBulletToEnemy(vector<Billboard*>* m_pBloodBillboard, GameObject* pEnemy, int& KillCount)
{

	FXMVECTOR BulletPos = XMLoadFloat3(&m_pCamera->GetPosition());

	FXMVECTOR BulletDir = XMLoadFloat3(&m_pCamera->GetLookVector());

	float dis = 0;
	bool crush = false;

	//1차 충돌 처리
	std::list<pair<AliensBoudingBox*, float>> crushlist;
	AliensBoudingBox EnmeyCollsion(pEnemy, ((AlienSpider*)(pEnemy))->m_MonsterScale);
	EnmeyCollsion.UpdateEntireBouding();
	EnmeyCollsion.UpdateBodyBouding();
	EnmeyCollsion.UPdateLegBounding();

		if (((AlienSpider*)(pEnemy))->m_GoalType != Deaded_Goal) {
		
			for (int i = 0; i < 6; i++) {
				if (crush == true)
					break;

				if (EnmeyCollsion.m_Bodys[i].Intersects(BulletPos, BulletDir, dis)) {
					HittedSound->Play(0, 0, 0);
					for (int i = 0; i < 5; ++i) {

						int idx = EffectIndex[rand() % 11];
						for (auto& paticle : m_pBloodBillboard[idx]) {
							if (!paticle->active) {
								paticle->active = true;
								paticle->m_CrashObject = &EnmeyCollsion.m_Bodys[i];
								break;
							}
						}
					}
					EnmeyCollsion.m_pOwner->m_bHitted = true;
					EnmeyCollsion.m_pOwner->m_HP -= ((Player*)m_pPlayer->m_pOwner)->GetBulletDamage();
					if (EnmeyCollsion.m_pOwner->m_HP <= 0) {
						++KillCount;
					}
					crush = true;

					break;
				}
			}

			for (int i = 0; i < 8; i++) {
				if (crush == true)
					break;
				if (EnmeyCollsion.m_legs[i].Intersects(BulletPos, BulletDir, dis)) {

					for (int i = 0; i < 5; ++i) {
						//int idx = rand() % 15;
						int idx = EffectIndex[rand() % 11];
						for (auto& paticle : m_pBloodBillboard[idx]) {
							if (!paticle->active) {
								paticle->active = true;
								paticle->m_CrashObject = &EnmeyCollsion.m_legs[i];
								break;
							}
						}
					}
					EnmeyCollsion.m_pOwner->m_bHitted = true;
					EnmeyCollsion.m_pOwner->m_HP -= ((Player*)m_pPlayer->m_pOwner)->GetBulletDamage();
					if (EnmeyCollsion.m_pOwner->m_HP <= 0) {
						++KillCount;
					}
					crush = true;

					break;
				}
			}

		}
	







		return crush;
}

void CollisionManager::CollisionBulletToItemBox(vector<Billboard*>& ExplosionEffect,vector<Item*>& Items)
{
	FXMVECTOR BulletPos = XMLoadFloat3(&m_pCamera->GetPosition());
	FXMVECTOR BulletDir = XMLoadFloat3(&m_pCamera->GetLookVector());

	float dis = 0;


	for (auto it = m_ItemBoxes.begin(); it != m_ItemBoxes.end(); ++it) {
		BOBBox* a = static_cast<BOBBox*>(*it);
		a->UpdateCollision();
		if (a->m_Transformboudingbox.Intersects(BulletPos, BulletDir, dis)) {
			HittedSound->Play(0, 0, 0);
			ShortExplosion->Play(0, 0, 0);
			a->m_pOwner->m_bActive = false;
			for (auto& bill : ExplosionEffect) {
				if (bill->active == false) {
					bill->SetPosition(a->m_pOwner->GetPosition());
					bill->active = true;
					break;
				}
			}

			for (auto& item : Items) {

					
				
					
				if (item->m_bActive == false) {
					item->m_bActive = true;
					switch (RandomItemType(gen)) {
					//switch (speed+1) {
					
					case sampling1:
						SetSamplingItem(item, a->m_pOwner->GetPosition(),1);
					break;
					case sampling2:
						SetSamplingItem(item, a->m_pOwner->GetPosition(), 2);
						break;
					case sampling3:
						SetSamplingItem(item, a->m_pOwner->GetPosition(), 3);
						break;
					case syringe:
						SetSyringeItem(item, a->m_pOwner->GetPosition());
						break;
					case shotgun:
						SetShotgunItem(item, a->m_pOwner->GetPosition());
						break;
					case machinegun:
						SetMachinegunItem(item, a->m_pOwner->GetPosition());
						break;
					case health:
						SetHealItem(item, a->m_pOwner->GetPosition());
						break;
					case damage:
						SetDamageItem(item, a->m_pOwner->GetPosition());
						break;
					case speed:
						SetSpeedItem(item, a->m_pOwner->GetPosition());
						break;
					case sheild:
						SetSheildItem(item, a->m_pOwner->GetPosition());
						break;
					}

					

					break;
				}

			}




			delete* it;
			m_ItemBoxes.erase(it);
			break;
		}
	}





}

void CollisionManager::SetSamplingItem(Item* pGameObject, const XMFLOAT3& pos, int num)
{

	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();

	if (num == 1) {
		pGameObject->SetItemType(sampling1);
		pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/sampling1.bin", "Model/Textures/Item/")->m_pModelRootObject;
	}
	else if (num == 2) {
		pGameObject->SetItemType(sampling1);
		pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/sampling1.bin", "Model/Textures/Item/")->m_pModelRootObject;
	}
	else {
		pGameObject->SetItemType(sampling1);
		pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/sampling1.bin", "Model/Textures/Item/")->m_pModelRootObject;

	}

	pGameObject->SetPosition(pos);
}

void CollisionManager::SetSyringeItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(syringe);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Item/syringe.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->Rotate(&XMFLOAT3(1, 0, 0), -90.f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));
}

void CollisionManager::SetShotgunItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(shotgun);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Weapon/shotgun.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(0.7f, 0.7f, 0.7f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));
}

void CollisionManager::SetMachinegunItem(Item* pGameObject, const XMFLOAT3& pos)
{
	pGameObject->m_xmf4x4ToParent = Matrix4x4::Identity();
	pGameObject->SetItemType(machinegun);
	pGameObject->m_pChild = m_pResourceManager->BringModelInfo("Model/Weapon/machinegun.bin", "Model/Textures/Item/")->m_pModelRootObject;
	pGameObject->SetScale(0.5f, 0.5f, 0.5f);
	pGameObject->Rotate(&XMFLOAT3(1,0,0),90.f);
	pGameObject->SetPosition(pos);
	pGameObject->AddPostion(XMFLOAT3(0, 0.5f, 0.0f));
}

void CollisionManager::RenderEnemyBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pObject)
{

	AliensBoudingBox a(pObject, pObject->m_MonsterScale);
	a.UpdateCollisionDetectBouding();
	a.UpdateEntireBouding();
	a.UpdateBodyBouding();
	a.UPdateLegBounding();
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

	XMFLOAT3 xmfloat3 = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);

	xmf4x4World = GetSphereMatrix(a.m_Obstable.Radius, a.m_Obstable.Center);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	if (m_psphere) m_psphere->Render(pd3dCommandList, 0);

	xmfloat3 = XMFLOAT3(1.0f, 0.0f, 0.0f);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);

	xmf4x4World = GetSphereMatrix(a.m_Entire.Radius, a.m_Entire.Center);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	if (m_psphere) m_psphere->Render(pd3dCommandList, 0);

	//세부 사항
	for (int i = 0; i < 6; i++) {
		xmf4x4World = GetSphereMatrix(a.m_Bodys[i].Radius, a.m_Bodys[i].Center);
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		if (m_psphere) m_psphere->Render(pd3dCommandList, 0);
	}

	for (int i = 0; i < 8; i++) {
		xmf4x4World = GetSphereMatrix(a.m_legs[i].Radius, a.m_legs[i].Center);
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		if (m_psphere) m_psphere->Render(pd3dCommandList, 0);
	}

}

void CollisionManager::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList)
{
	
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	int BoundingMeshCount = 0;
	if (m_pPlayer->m_pOwner->m_PlayerInPlace == MainPlace) {

		for (auto& SM : m_StaticObjects) {
			m_BoundingBoxMeshes[BoundingMeshCount]->UpdateVertexPosition(&((BOBBox*)SM)->m_Transformboudingbox);
			m_BoundingBoxMeshes[BoundingMeshCount++]->Render(pd3dCommandList);
		}

	}

	if (m_pPlayer->m_pOwner->m_PlayerInPlace == SubPlace)
	for (int i = 0; i < m_SubStaticObjects.size(); ++i) {
		m_SubBoundingBoxMeshes[i]->Render(pd3dCommandList);
	}


	//총알만 업데이트
	for (auto& SM : m_bullets) {
		if (SM->m_pOwner->m_bActive) {
			m_BoundingBoxMeshes[BoundingMeshCount]->UpdateVertexPosition(&((BOBBox*)SM)->m_Transformboudingbox);
			m_BoundingBoxMeshes[BoundingMeshCount++]->Render(pd3dCommandList);
		}
	}




	//아이템 바운딩박스 업데이트
	for (auto& SM : m_ItemBoxes) {
		if (SM->m_pOwner->m_bActive) {
			((BOBBox*)SM)->UpdateCollision();
			m_BoundingBoxMeshes[BoundingMeshCount]->UpdateVertexPosition(&((BOBBox*)SM)->m_Transformboudingbox);
			m_BoundingBoxMeshes[BoundingMeshCount++]->Render(pd3dCommandList);
		}
	}


	 xmf4x4World = m_pPlayer->m_pOwner->m_xmf4x4World;
	 xmf4x4World._41 += ((BCapsule*)(m_pPlayer))->m_center.x;
	 xmf4x4World._42 += ((BCapsule*)(m_pPlayer))->m_center.y;
	 xmf4x4World._43 += ((BCapsule*)(m_pPlayer))->m_center.z;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);



	if (m_pPlayerCapsuleMesh) m_pPlayerCapsuleMesh->Render(pd3dCommandList, 0);
	 XMFLOAT3 xmfloat3 = XMFLOAT3(0, 1, 0);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
	xmf4x4World = GetSphereMatrix((BCapsule*)m_pPlayer);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	if(m_psphere) m_psphere->Render(pd3dCommandList,0);



	for (const auto& a : m_DogEnemy) {
		a->UpdateCollision();
		xmf4x4World = GetSphereMatrix(a->m_boundingshpere.Radius, a->m_boundingshpere.Center);
		XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
		pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
		m_psphere->Render(pd3dCommandList, 0);
	}


	//적의 바운디 박스
	for (const auto& a : m_EnemyObjects) {
		RenderEnemyBoundingBox(pd3dCommandList,a->m_pOwner);
	}

	
	xmfloat3 = XMFLOAT3(0.0f, 0.0, 1.0f);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
	m_pRedZoneCollision->UpdateCollision();
	xmf4x4World = GetSphereMatrix(m_pRedZoneCollision->m_boundingshpere.Radius, m_pRedZoneCollision->m_boundingshpere.Center);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	m_psphere->Render(pd3dCommandList, 0);



	//BossRendering
	BSphere m_top(XMFLOAT3(0,1.5, 0), 1.5f , m_pBossMonster);
	m_top.UpdateCollision();
	BSphere m_bottom(XMFLOAT3(0,0.7,0), 1.0f , m_pBossMonster);
	m_bottom.UpdateCollision();

	xmf4x4World = GetSphereMatrix(m_top.m_boundingshpere.Radius, m_top.m_boundingshpere.Center);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	m_psphere->Render(pd3dCommandList, 0);
	xmf4x4World = GetSphereMatrix(m_bottom.m_boundingshpere.Radius, m_bottom.m_boundingshpere.Center);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

	m_psphere->Render(pd3dCommandList, 0);
}

CapsuleMesh::CapsuleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nslice, int nstack, float radius, float tip, float base) :
	m_nSlices(nslice),
	m_nStacks(nstack),
	m_fRadius(radius)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_nVertices = 2 + (m_nSlices * (m_nStacks - 1));
				//꼭대기 ,아래,  상, 하 
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	//180도를 nStacks 만큼 분할한다. 
	float fDeltaPhi = float(XM_PI / m_nStacks);
	//360도를 nSlices 만큼 분할한다. 
	float fDeltaTheta = float((2.0f * XM_PI) / m_nSlices);

	int k = 0;

	float theta_i, phi_j;

	m_pxmf3Positions[k++] = XMFLOAT3(0.0f, +m_fRadius + tip, 0.0f);

	for (int j = 1; j < m_nStacks; ++j) {

		phi_j = fDeltaPhi * j;
		for (int i = 0; i < m_nSlices; i++)
		{
			theta_i = fDeltaTheta * i;

			if (phi_j < 90 * (XM_PI / 180)) {
				m_pxmf3Positions[k++] = XMFLOAT3(m_fRadius * sinf(phi_j) * cosf(theta_i),
					m_fRadius * cosf(phi_j) + tip, m_fRadius * sinf(phi_j) * sinf(theta_i));
			}
			else {
				m_pxmf3Positions[k++] = XMFLOAT3(m_fRadius * sinf(phi_j) * cosf(theta_i),
					m_fRadius * cosf(phi_j) + base, m_fRadius * sinf(phi_j) * sinf(theta_i));
			}


		}

	}
	m_pxmf3Positions[k++] = XMFLOAT3(0.0f, -m_fRadius + base, 0.0f);

	m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions,
		sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pd3dPositionUploadBuffer);

	m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_nSubMeshes = 1;

	int nIndices = (m_nSlices * 3) * 2 + (m_nSlices * (m_nStacks - 2) * 3 * 2);

	m_pnSubSetIndices = new int[m_nSubMeshes];
	m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

	m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
	m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];


	m_pnSubSetIndices[0] = nIndices;
	m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];
	k = 0;

	//구의 위쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < m_nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = 0;
		m_ppnSubSetIndices[0][k++] = 1 + ((i + 1) % m_nSlices);
		m_ppnSubSetIndices[0][k++] = 1 + i;
	}
	//구의 원기둥의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int j = 0; j < m_nStacks - 2; j++)
	{
		for (int i = 0; i < m_nSlices; i++)
		{
			//사각형의 첫 번째 삼각형의 인덱스이다. 
			m_ppnSubSetIndices[0][k++] = 1 + (i + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * m_nSlices));
			//사각형의 두 번째 삼각형의 인덱스이다.
			m_ppnSubSetIndices[0][k++] = 1 + (i + ((j + 1) * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + (j * m_nSlices));
			m_ppnSubSetIndices[0][k++] = 1 + (((i + 1) % m_nSlices) + ((j + 1) * m_nSlices));
		}
	}


	//구의 아래쪽 원뿔의 표면을 표현하는 삼각형들의 인덱스이다. 
	for (int i = 0; i < m_nSlices; i++)
	{
		m_ppnSubSetIndices[0][k++] = (m_nVertices - 1);
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - m_nSlices) + i;
		m_ppnSubSetIndices[0][k++] = ((m_nVertices - 1) - m_nSlices) + ((i + 1) % m_nSlices);
	}



	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);
	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];




}


