#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"
#include "ColonyPlayer.h"


class Collision {
public:
	Collision() {};
	Collision(GameObject* pOwner):m_pOwner(pOwner){

	};
	virtual ~Collision() {};
	 
	 BasicMesh* m_pMesh = NULL;
	 GameObject* m_pOwner = NULL;

	 void BoundingRender(ID3D12GraphicsCommandList* pd3dCommandList) {
		 if (m_pMesh) m_pMesh->Render(pd3dCommandList,0);
	 }

	 virtual void UpdateCollision() {

	 }
};

// 방향성 박스
class BOBBox :public Collision {
public:
	BoundingOrientedBox m_boundingbox{};
	XMFLOAT3 m_center{};
	XMFLOAT4X4 mxf4x4Position{};
public:
	BOBBox(XMFLOAT3 center, XMFLOAT3  extend, GameObject* pOwner) :Collision(pOwner) {
		m_boundingbox = BoundingOrientedBox(center, extend, XMFLOAT4(0, 0, 0, 1));
		if (m_pOwner) mxf4x4Position = m_pOwner->m_xmf4x4World;
	};


	virtual void UpdateCollision() {
		XMFLOAT4X4 xmatrix = m_pOwner->m_xmf4x4World;
		xmatrix._41 += m_center.x;
		xmatrix._42 += m_center.y;
		xmatrix._43 += m_center.z;

		m_boundingbox.Transform(m_boundingbox, DirectX::XMLoadFloat4x4(&xmatrix));
	}


};
// 구
class BSphere :public Collision {
public:
	BoundingSphere m_boundingshpere;
	XMFLOAT3 m_center;
	float	m_radius;
public:
	BSphere(XMFLOAT3 center ,float radius, GameObject* pOwner) :Collision(pOwner) {
		m_center = center;
		m_radius = radius;
	}

	virtual void UpdateCollision() {
		XMFLOAT4X4 xmatrix = m_pOwner->m_xmf4x4World;
		xmatrix._41 += m_center.x;
		xmatrix._42 += m_center.y;
		xmatrix._43 += m_center.z;

		m_boundingshpere.Transform(m_boundingshpere, DirectX::XMLoadFloat4x4(&xmatrix));
	}
};
// 캡슐 
class BCapsule :public Collision {
public:
	BoundingSphere m_boundingshpere;
	float m_tip = 1.0f;
	float m_base = 0.0f;
	float m_radius = 0.0f;
	XMFLOAT3 m_center{};
public:
	BCapsule(XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner) :Collision(pOwner) {
		m_tip = tip;
		m_base = base;
		m_boundingshpere.Radius = radius;
		m_radius = radius;
		m_center = center;
	}

	virtual void UpdateCollision() {
		XMFLOAT4X4 xmatrix = m_pOwner->m_xmf4x4World;
		xmatrix._41 += m_center.x;
		xmatrix._42 += m_center.y;
		xmatrix._43 += m_center.z;

		m_boundingshpere.Transform(m_boundingshpere, DirectX::XMLoadFloat4x4(&xmatrix));
	}


	BoundingSphere& GetCapsuleBounding(const XMFLOAT3& OtherCeter){
		
		//위치 담기
		XMFLOAT3 m_pos = XMFLOAT3(m_pOwner->m_xmf4x4World._41 + m_center.x, m_pOwner->m_xmf4x4World._42 + m_center.y, m_pOwner->m_xmf4x4World._43 + m_center.z);
		//밑
		XMFLOAT3 xm3A = XMFLOAT3(m_pos.x, m_pos.y + m_base, m_pos.z);
		//위
		XMFLOAT3 xm3B = XMFLOAT3(m_pos.x, m_pos.y + m_tip, m_pos.z);
		//외각 점
		XMFLOAT3 xm3C = OtherCeter;


		XMFLOAT3 xmAB = Vector3::Subtract(xm3B, xm3A);
		XMFLOAT3 xmAC = Vector3::Subtract(xm3C, xm3A);

		float t = Vector3::DotProduct(xmAC, xmAB);

		if (t <= 0.0f) {
			t = 0.0f;
			m_boundingshpere.Center = xm3A;
		}
		else {
			float denom = Vector3::DotProduct(xmAB, xmAB);
			if (t >= denom) {

				t = 1.0f;
				m_boundingshpere.Center = xm3B;

			}
			else {

				t = t / denom;
				m_boundingshpere.Center = Vector3::Add(xm3A, Vector3::ScalarProduct(xmAB, t, false));
				
			}


		}


		return m_boundingshpere;
	}

	BoundingSphere GetCapsuleBounding(const BOBBox& OBB) {
		XMFLOAT4X4 boxTransform = OBB.mxf4x4Position;
		
		XMMATRIX boxRotation = XMLoadFloat4x4(&boxTransform);
		XMMATRIX inverseBoxRotation = ::XMMatrixInverse(nullptr, boxRotation);

		// 캡슐의 세계 좌표계 위치 계산
		XMFLOAT3 m_pos = XMFLOAT3(m_pOwner->m_xmf4x4World._41 + m_center.x, m_pOwner->m_xmf4x4World._42 + m_center.y, m_pOwner->m_xmf4x4World._43 + m_center.z); // 캡슐 중심 위치
	
		// 캡슐의 끝점 좌표
		XMFLOAT3 xm3A = XMFLOAT3(m_pos.x, m_pos.y + m_base, m_pos.z); // 밑
		XMFLOAT3 xm3B = XMFLOAT3(m_pos.x, m_pos.y + m_tip, m_pos.z);  // 위

		//string temp = "x  ";
		//temp += to_string(xm3A.x);
		//temp += "y  ";
		//temp += to_string(xm3A.y);
		//temp += "z  ";
		//temp += to_string(xm3A.z);
		//temp += "\n";
		//OutputDebugStringA(temp.c_str());

		// 캡슐의 끝점을 박스의 로컬 좌표계로 변환
		XMVECTOR xm3A_local = ::XMVector3Transform(XMLoadFloat3(&xm3A), inverseBoxRotation);
		XMVECTOR xm3B_local = ::XMVector3Transform(XMLoadFloat3(&xm3B), inverseBoxRotation);
		XMVECTOR xm3C_local = ::XMVector3Transform(XMLoadFloat3(&OBB.m_center), inverseBoxRotation);

		// 로컬 좌표로 변환된 벡터를 다시 XMFLOAT3로 변환
		XMFLOAT3 xm3A_local_float, xm3B_local_float, xm3C_local_float;
		XMStoreFloat3(&xm3A_local_float, xm3A_local);
		XMStoreFloat3(&xm3B_local_float, xm3B_local);
		XMStoreFloat3(&xm3C_local_float, xm3C_local);

		// 벡터 연산
		XMFLOAT3 xmAB = Vector3::Subtract(xm3B_local_float, xm3A_local_float);
		XMFLOAT3 xmAC = Vector3::Subtract(xm3C_local_float, xm3A_local_float);

		// 내적을 통한 최근접 점 계산
		float t = Vector3::DotProduct(xmAC, xmAB);
		BoundingSphere boundingsphere;
		boundingsphere.Radius = m_radius;
		if (t <= 0.0f) {
			t = 0.0f;
			boundingsphere.Center = xm3A_local_float;
		}
		else {
			float denom = Vector3::DotProduct(xmAB, xmAB);
			if (t >= denom) {
				t = 1.0f;
				boundingsphere.Center = xm3B_local_float;
			}
			else {
				t = t / denom;
				boundingsphere.Center = Vector3::Add(xm3A_local_float, Vector3::ScalarProduct(xmAB, t, false));
			}
		}

		// 최종 계산된 중심을 월드 좌표로 변환
		XMVECTOR worldCenter = XMVector3Transform(XMLoadFloat3(&boundingsphere.Center), boxRotation);
		XMStoreFloat3(&boundingsphere.Center, worldCenter);



		return boundingsphere;
	}
	
};

class CapsuleMesh : public BasicMesh
{
public:
	CapsuleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nslice, int nstack, float radius,float tip,float base);
	virtual ~CapsuleMesh() {};


	int m_nSlices = 10;
	int	m_nStacks = 3;
	float m_fRadius;


};
//복합 콜리젼
class AliensBoudingBox:public Collision {
public:
	AliensBoudingBox(GameObject* pOwner) :Collision(pOwner) {
		m_Entire.Radius = 1.5f; 
		m_Bodys[0].Radius = 0.3f; //DEF_SPINE_1
		m_Bodys[1].Radius = 0.3f; //DEF_CHEST
		m_Bodys[2].Radius = 0.2f; //DEF_NECK_1
		m_Bodys[3].Radius = 0.2f; //DEF_HEAD
		m_Bodys[4].Radius = 0.4f; //DEF_TAIL
		m_Bodys[5].Radius = 0.4f; //DEF_TAIL_001; 

		m_legs[0].Radius = 0.2f; //DEF_SPINE_1
		m_legs[1].Radius = 0.2f; //DEF_CHEST
		m_legs[2].Radius = 0.2f; //DEF_NECK_1
		m_legs[3].Radius = 0.2f; //DEF_HEAD
		m_legs[4].Radius = 0.2f; //DEF_TAIL
		m_legs[5].Radius = 0.2f; //DEF_TAIL_001; 
		m_legs[6].Radius = 0.2f; //DEF_TAIL_001; 
		m_legs[7].Radius = 0.2f; //DEF_TAIL_001; 
	}
public:
	BoundingSphere m_Entire; //DEF-Hip의 정보를 가져옴
	BoundingSphere m_Bodys[6]; // DEF_SPINE_1 ,DEF_CHEST ,DEF_NECK_1 ,DEF_HEAD , DEF_TAIL ,DEF_TAIL_001
	BoundingSphere m_legs[8]; // DEF_LEG_BACK_02_L			DEF_LEG_BACK_02_R		DEF_LEG_FRONT_02_L  
							//DEF_LEG_FRONT_02_R		DEF_LEG_MIDDLE_02_L			DEF_LEG_MIDDLE_02_R
							// DEF_FORARM_L   DEF_FORARM_R
	void UpdateEntireBouding() {
		
		
		XMFLOAT3 pos;

		pos = m_pOwner->FramePos[AlienboneIndex::DEF_HIPS];
		m_Entire.Center = pos;
		m_Entire.Center.y += 0.75f;
	}

	void UpdateBodyBouding() {

		for (int i = 0; i < 6; ++i) {
			int AnimationIndex = 0;
			switch (i) {
			case 0:
				AnimationIndex = AlienboneIndex::DEF_SPINE_1;
				break;
			case 1:
				AnimationIndex = AlienboneIndex::DEF_CHEST;
				break;
			case 2:
				AnimationIndex = AlienboneIndex::DEF_NECK_1;
				break;
			case 3:
				AnimationIndex = AlienboneIndex::DEF_HEAD;
				break;
			case 4:
				AnimationIndex = AlienboneIndex::DEF_TAIL;
				break;
			case 5:
				AnimationIndex = AlienboneIndex::DEF_TAIL_001;
				break;
			}

			m_Bodys[i].Center = m_pOwner->FramePos[AnimationIndex];
		}
	}

	void UPdateLegBounding() {
		for (int i = 0; i < 8; ++i) {
			int AnimationIndex = 0;
			switch (i) {
			case 0:
				AnimationIndex = AlienboneIndex::DEF_LEG_BACK_02_L;
				break;
			case 1:
				AnimationIndex = AlienboneIndex::DEF_LEG_BACK_02_R;
				break;
			case 2:
				AnimationIndex = AlienboneIndex::DEF_LEG_FRONT_02_L;
				break;
			case 3:
				AnimationIndex = AlienboneIndex::DEF_LEG_FRONT_02_R;
				break;
			case 4:
				AnimationIndex = AlienboneIndex::DEF_LEG_MIDDLE_02_L;
				break;
			case 5:
				AnimationIndex = AlienboneIndex::DEF_LEG_MIDDLE_02_R;
				break;
			case 6:
				AnimationIndex = AlienboneIndex::DEF_FORARM_L;
				break;
			case 7:
				AnimationIndex = AlienboneIndex::DEF_FORARM_R;
				break;
			}
			m_legs[i].Center = m_pOwner->FramePos[AnimationIndex];
		}


	}

};

class CollisionManager{
public:

	std::vector<Collision*> m_StaticObjects;
	//Accel Obeject       Dynamic Size  -> list 
	std::list<Collision*> m_AccelationObjects;

	std::list<AliensBoudingBox*> m_EnemyObjects;

	//Player 
	Collision* m_pPlayer;
	Camera* m_pCamera;


	//Rendering
	std::vector<BoundingBoxMesh*> m_BoundingBoxMeshes;
	int boundingcur = 0 ;
	CapsuleMesh* m_pCapsuleMesh=NULL;
	ShphereMesh* m_psphere=NULL;

	CapsuleMesh* m_pPlayerCapsuleMesh=NULL;
	XMFLOAT3 m_PlayerCapsulePos;

	int EffectIndex[11] = { 0,1,2,5,8,9,10,11,12,13,16 };
public:

	CollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	~CollisionManager();
	XMFLOAT4X4 MakeScaleMatrix(float x , float y , float z);
	XMFLOAT4X4 GetSphereMatrix(BSphere* pSphere);
	XMFLOAT4X4 GetSphereMatrix(BCapsule* pSphere); //캡슐의 원
	XMFLOAT4X4 GetSphereMatrix(const float& r, const XMFLOAT3& center);
	//등록 박스
	void EnrollObjectIntoBox(bool isAccel,XMFLOAT3 center, XMFLOAT3  extend, GameObject* pOwner);
	//등록 구
	void EnrollObjectIntoSphere(bool isAccel, XMFLOAT3 center,float radius, GameObject* pOwner);

	void EnrollObjectIntoCapsule(bool isAccel, XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner);

	void EnrollPlayerIntoCapsule(XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner);

	void EnrollEnemy(GameObject* pEnemy);

	void LoadCollisionBoxInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* filename);
	
	void EnrollBulletDir(Camera* pCamera) {m_pCamera = pCamera;}

	void CheckVisiableEnemy();


	//충돌 처리 함수들
	bool CollisionPlayerToStaticObeject();
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList);
	bool CollsionBulletToEnemy(vector<Billboard*>* m_pBloodBillboard);
	void CollisionEnemyToStaticObeject();    


	void ReleaseUploadBuffers();
};

