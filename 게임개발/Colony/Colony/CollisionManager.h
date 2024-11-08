#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"
#include "ColonyPlayer.h"
#include "ResourceManager.h"
#include "SoundManager.h"

class Collision {
public:
	Collision() {};
	Collision(GameObject* pOwner):m_pOwner(pOwner){

	};
	Collision(GameObject* pOwner,float scale) :m_pOwner(pOwner) {

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

// ���⼺ �ڽ�
class BOBBox :public Collision {
public:
	BoundingOrientedBox m_boundingbox{};
	BoundingOrientedBox m_Transformboudingbox{};

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
		
		m_boundingbox.Transform(m_Transformboudingbox, DirectX::XMLoadFloat4x4(&xmatrix));
	}


};
// ��
class BSphere :public Collision {
public:
	BoundingSphere m_boundingshpere;
	XMFLOAT3 m_center;
	float	m_radius;
public:
	BSphere(XMFLOAT3 center ,float radius, GameObject* pOwner) :Collision(pOwner) {
		m_center = center;
		m_radius = radius;
		m_boundingshpere.Radius = m_radius;
		m_boundingshpere.Center = m_center;
	}

	virtual void UpdateCollision() {
		XMFLOAT4X4 xmatrix = m_pOwner->m_xmf4x4World;
		xmatrix._41 += m_center.x;
		xmatrix._42 += m_center.y;
		xmatrix._43 += m_center.z;
		BoundingSphere boundingshpere(m_center, m_radius);
		boundingshpere.Transform(m_boundingshpere, DirectX::XMLoadFloat4x4(&xmatrix));
	}
};
// ĸ�� 
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
		
		//��ġ ���
		XMFLOAT3 m_pos = XMFLOAT3(((Player*)m_pOwner)->m_xmf3Position.x + m_center.x, ((Player*)m_pOwner)->m_xmf3Position.y+ m_center.y, ((Player*)m_pOwner)->m_xmf3Position.z + m_center.z);
		//��
		XMFLOAT3 xm3A = XMFLOAT3(m_pos.x, m_pos.y + m_base, m_pos.z);
		//��
		XMFLOAT3 xm3B = XMFLOAT3(m_pos.x, m_pos.y + m_tip, m_pos.z);
		//�ܰ� ��
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

		// ĸ���� ���� ��ǥ�� ��ġ ���
		XMFLOAT3 m_pos = XMFLOAT3(m_pOwner->m_xmf4x4World._41 + m_center.x, m_pOwner->m_xmf4x4World._42 + m_center.y, m_pOwner->m_xmf4x4World._43 + m_center.z); // ĸ�� �߽� ��ġ
	
		// ĸ���� ���� ��ǥ
		XMFLOAT3 xm3A = XMFLOAT3(m_pos.x, m_pos.y + m_base, m_pos.z); // ��
		XMFLOAT3 xm3B = XMFLOAT3(m_pos.x, m_pos.y + m_tip, m_pos.z);  // ��

		//string temp = "x  ";
		//temp += to_string(xm3A.x);
		//temp += "y  ";
		//temp += to_string(xm3A.y);
		//temp += "z  ";
		//temp += to_string(xm3A.z);
		//temp += "\n";
		//OutputDebugStringA(temp.c_str());

		// ĸ���� ������ �ڽ��� ���� ��ǥ��� ��ȯ
		XMVECTOR xm3A_local = ::XMVector3Transform(XMLoadFloat3(&xm3A), inverseBoxRotation);
		XMVECTOR xm3B_local = ::XMVector3Transform(XMLoadFloat3(&xm3B), inverseBoxRotation);
		XMVECTOR xm3C_local = ::XMVector3Transform(XMLoadFloat3(&OBB.m_center), inverseBoxRotation);

		// ���� ��ǥ�� ��ȯ�� ���͸� �ٽ� XMFLOAT3�� ��ȯ
		XMFLOAT3 xm3A_local_float, xm3B_local_float, xm3C_local_float;
		XMStoreFloat3(&xm3A_local_float, xm3A_local);
		XMStoreFloat3(&xm3B_local_float, xm3B_local);
		XMStoreFloat3(&xm3C_local_float, xm3C_local);

		// ���� ����
		XMFLOAT3 xmAB = Vector3::Subtract(xm3B_local_float, xm3A_local_float);
		XMFLOAT3 xmAC = Vector3::Subtract(xm3C_local_float, xm3A_local_float);

		// ������ ���� �ֱ��� �� ���
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

		// ���� ���� �߽��� ���� ��ǥ�� ��ȯ
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
//���� �ݸ���
class AliensBoudingBox:public Collision {
public:
	AliensBoudingBox() {};
	AliensBoudingBox(GameObject* pOwner) :Collision(pOwner) {
		m_Obstable.Radius = 0.8f;
		m_Entire.Radius = 1.0f;
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
	AliensBoudingBox(GameObject* pOwner,float scale) :Collision(pOwner) {
		m_Obstable.Radius = 0.8f * scale;
		m_Entire.Radius = 1.0f * scale;
		m_Bodys[0].Radius = 0.3f * scale; //DEF_SPINE_1
		m_Bodys[1].Radius = 0.3f * scale; //DEF_CHEST
		m_Bodys[2].Radius = 0.2f * scale; //DEF_NECK_1
		m_Bodys[3].Radius = 0.2f * scale; //DEF_HEAD
		m_Bodys[4].Radius = 0.4f * scale; //DEF_TAIL
		m_Bodys[5].Radius = 0.4f * scale; //DEF_TAIL_001; 

		m_legs[0].Radius = 0.2f * scale; //DEF_SPINE_1
		m_legs[1].Radius = 0.2f * scale; //DEF_CHEST
		m_legs[2].Radius = 0.2f * scale; //DEF_NECK_1
		m_legs[3].Radius = 0.2f * scale; //DEF_HEAD
		m_legs[4].Radius = 0.2f * scale; //DEF_TAIL
		m_legs[5].Radius = 0.2f * scale; //DEF_TAIL_001; 
		m_legs[6].Radius = 0.2f * scale; //DEF_TAIL_001; 
		m_legs[7].Radius = 0.2f * scale; //DEF_TAIL_001; 
	}

public:
	BoundingSphere m_Obstable; 
	BoundingSphere m_Entire; //DEF-Hip�� ������ ������
	BoundingSphere m_Bodys[6]; // DEF_SPINE_1 ,DEF_CHEST ,DEF_NECK_1 ,DEF_HEAD , DEF_TAIL ,DEF_TAIL_001
	BoundingSphere m_legs[8]; // DEF_LEG_BACK_02_L			DEF_LEG_BACK_02_R		DEF_LEG_FRONT_02_L  
							//DEF_LEG_FRONT_02_R		DEF_LEG_MIDDLE_02_L			DEF_LEG_MIDDLE_02_R
							// DEF_FORARM_L   DEF_FORARM_R
	void UpdateEntireBouding() {
		
		
		XMFLOAT3 pos = XMFLOAT3(m_pOwner->m_xmf4x4ToParent._41, m_pOwner->m_xmf4x4ToParent._42, m_pOwner->m_xmf4x4ToParent._43);
		m_Entire.Center = pos;
		m_Entire.Center.y +=m_pOwner->m_MonsterScale;
	}

	void UpdateCollisionDetectBouding() {


		XMFLOAT3 pos;

		pos = m_pOwner->FramePos[AlienboneIndex::DEF_HIPS];
		XMFLOAT3 offset = XMFLOAT3(0.0, 0.0, 1.0f);
		m_Obstable.Center = pos;
		XMFLOAT3 LookVector = m_pOwner->GetLook();
		LookVector = Vector3::ScalarProduct(LookVector, 2.0f* m_pOwner->m_MonsterScale, true);
		m_Obstable.Center = Vector3::Add(m_Obstable.Center, LookVector);

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

class BossBoundingBox : public Collision {
public:
	BossBoundingBox() {};
	BossBoundingBox(GameObject* pOwner) {
		float scale = pOwner->m_MonsterScale;
		Grenadier_EyeArmor = pOwner->FindFrame("Grenadier_EyeArmor");

		Grenadier_LeftUpperLeg = pOwner->FindFrame("Grenadier_LeftUpperLeg");
		Grenadier_LeftLowerLeg = pOwner->FindFrame("Grenadier_LeftLowerLeg");
		Grenadier_LeftFoot = pOwner->FindFrame("Grenadier_LeftFoot");

		Grenadier_RightUpperLeg = pOwner->FindFrame("Grenadier_RightUpperLeg");
		Grenadier_RightLowerLeg = pOwner->FindFrame("Grenadier_RightLowerLeg");
		Grenadier_RightFoot = pOwner->FindFrame("Grenadier_RightFoot");

		Grenadier_LeftHandEnd = pOwner->FindFrame("Grenadier_LeftHandEnd");
		Grenadier_LeftUpperArm = pOwner->FindFrame("Grenadier_LeftUpperArm");
		Grenadier_LeftHand = pOwner->FindFrame("Grenadier_LeftHand");
	
		Grenadier_RightHandEnd = pOwner->FindFrame("Grenadier_RightHandEnd");
		Grenadier_RightUpperArm = pOwner->FindFrame("Grenadier_RightUpperArm");
		Grenadier_RightHand = pOwner->FindFrame("Grenadier_RightHand");

		m_Top.Radius = 1.0f * scale;

		m_RightLeg[0].Radius = 0.3f* scale;
		m_RightLeg[1].Radius = 0.3f* scale;
		m_RightLeg[2].Radius = 0.3f* scale;

		m_LeftLeg[0].Radius = 0.3f* scale;
		m_LeftLeg[1].Radius = 0.3f* scale;
		m_LeftLeg[2].Radius = 0.3f* scale;

		m_RightHand[0].Radius = 0.3f* scale;
		m_RightHand[1].Radius = 0.4f* scale;
		m_RightHand[2].Radius = 0.4f* scale;
		
		m_LeftHand[0].Radius = 0.3f* scale;
		m_LeftHand[1].Radius = 0.4f* scale;
		m_LeftHand[2].Radius = 0.4f* scale;



		m_pOwner = pOwner;

		m_Boundinglist.push_back(&m_Top);
		m_Framelist.push_back(Grenadier_EyeArmor);

		m_Framelist.push_back(Grenadier_RightHandEnd);
		m_Framelist.push_back(Grenadier_LeftHandEnd);

		m_Framelist.push_back(Grenadier_RightUpperArm);
		m_Framelist.push_back(Grenadier_LeftUpperArm);

		m_Framelist.push_back(Grenadier_RightHand);
		m_Framelist.push_back(Grenadier_LeftHand);


		m_Framelist.push_back(Grenadier_RightUpperLeg);
		m_Framelist.push_back(Grenadier_LeftUpperLeg);

		m_Framelist.push_back(Grenadier_RightLowerLeg);
		m_Framelist.push_back(Grenadier_LeftLowerLeg);

		m_Framelist.push_back(Grenadier_RightFoot);
		m_Framelist.push_back(Grenadier_LeftFoot);

		for (int i = 0; i < 3; i++) {
			m_Boundinglist.push_back(&m_RightHand[i]);
		
			m_Boundinglist.push_back(&m_LeftHand[i]);
		}

		for (int i = 0; i < 3; i++) {
			m_Boundinglist.push_back(&m_RightLeg[i]);
			m_Boundinglist.push_back(&m_LeftLeg[i]);
		}

	};
	std::list<BoundingSphere*> m_Boundinglist;
	std::vector<GameObject*> m_Framelist;
	GameObject* Grenadier_EyeArmor;

	GameObject* Grenadier_LeftUpperLeg;
	GameObject* Grenadier_LeftLowerLeg;
	GameObject* Grenadier_LeftFoot;

	GameObject* Grenadier_RightUpperLeg;
	GameObject* Grenadier_RightLowerLeg;
	GameObject* Grenadier_RightFoot;


	GameObject* Grenadier_LeftHandEnd;
	GameObject* Grenadier_LeftUpperArm;
	GameObject* Grenadier_LeftHand;

	GameObject* Grenadier_RightHandEnd;
	GameObject* Grenadier_RightUpperArm;
	GameObject* Grenadier_RightHand;

	BoundingSphere m_Top;
	BoundingSphere m_RightLeg[3];
	BoundingSphere m_LeftLeg[3];

	BoundingSphere m_RightHand[3];
	BoundingSphere m_LeftHand[3];


	int m_Critical = 0;

	void UpdateCollision() {
		
		m_pOwner->UpdateTransform(NULL);

		m_Top.Center = XMFLOAT3(Grenadier_EyeArmor->m_xmf4x4World._41,
								Grenadier_EyeArmor->m_xmf4x4World._42,
								Grenadier_EyeArmor->m_xmf4x4World._43);



		m_RightLeg[0].Center = XMFLOAT3(Grenadier_LeftUpperLeg->m_xmf4x4World._41,
			Grenadier_LeftUpperLeg->m_xmf4x4World._42,
			Grenadier_LeftUpperLeg->m_xmf4x4World._43);
		m_RightLeg[1].Center = XMFLOAT3(Grenadier_LeftLowerLeg->m_xmf4x4World._41,
			Grenadier_LeftLowerLeg->m_xmf4x4World._42,
			Grenadier_LeftLowerLeg->m_xmf4x4World._43);
		m_RightLeg[2].Center = XMFLOAT3(Grenadier_LeftFoot->m_xmf4x4World._41,
			Grenadier_LeftFoot->m_xmf4x4World._42,
			Grenadier_LeftFoot->m_xmf4x4World._43);



		m_LeftLeg[0].Center = XMFLOAT3(Grenadier_RightUpperLeg->m_xmf4x4World._41,
			Grenadier_RightUpperLeg->m_xmf4x4World._42,
			Grenadier_RightUpperLeg->m_xmf4x4World._43);
		m_LeftLeg[1].Center = XMFLOAT3(Grenadier_RightLowerLeg->m_xmf4x4World._41,
			Grenadier_RightLowerLeg->m_xmf4x4World._42,
			Grenadier_RightLowerLeg->m_xmf4x4World._43);
		m_LeftLeg[2].Center = XMFLOAT3(Grenadier_RightFoot->m_xmf4x4World._41,
			Grenadier_RightFoot->m_xmf4x4World._42,
			Grenadier_RightFoot->m_xmf4x4World._43);

		
		m_RightHand[0].Center = XMFLOAT3(Grenadier_RightHandEnd->m_xmf4x4World._41,
			Grenadier_RightHandEnd->m_xmf4x4World._42,
			Grenadier_RightHandEnd->m_xmf4x4World._43);
		m_RightHand[1].Center = XMFLOAT3(Grenadier_RightUpperArm->m_xmf4x4World._41,
			Grenadier_RightUpperArm->m_xmf4x4World._42,
			Grenadier_RightUpperArm->m_xmf4x4World._43);
		m_RightHand[2].Center = XMFLOAT3(Grenadier_RightHand->m_xmf4x4World._41,
			Grenadier_RightHand->m_xmf4x4World._42,
			Grenadier_RightHand->m_xmf4x4World._43);


		m_LeftHand[0].Center = XMFLOAT3(Grenadier_LeftHandEnd->m_xmf4x4World._41,
			Grenadier_LeftHandEnd->m_xmf4x4World._42,
			Grenadier_LeftHandEnd->m_xmf4x4World._43);
		m_LeftHand[1].Center = XMFLOAT3(Grenadier_LeftUpperArm->m_xmf4x4World._41,
			Grenadier_LeftUpperArm->m_xmf4x4World._42,
			Grenadier_LeftUpperArm->m_xmf4x4World._43);
		m_LeftHand[2].Center = XMFLOAT3(Grenadier_LeftHand->m_xmf4x4World._41,
			Grenadier_LeftHand->m_xmf4x4World._42,
			Grenadier_LeftHand->m_xmf4x4World._43);



	 }

	std::list<BoundingSphere*> GetBoundingList() {
		 
		return m_Boundinglist;
	}

};

class CollisionManager{
public:

	std::vector<Collision*> m_StaticObjects;
	std::vector<Collision*> m_SubStaticObjects;
	std::vector<Collision*> m_ItemBoxes;
	//Accel Obeject       Dynamic Size  -> list 
	std::list<Collision*> m_AccelationObjects;
	std::list<AliensBoudingBox*> m_EnemyObjects;
	std::list<BSphere*>			m_DogEnemy;

	std::vector<Collision*> m_bullets;


	BSphere* m_pRedZoneCollision;

	//Player 
	Collision* m_pPlayer;
	Camera* m_pCamera;
	//Boss
	BossBoundingBox* m_BoundMonster;

	//Rendering
	std::vector<BoundingBoxMesh*> m_BoundingBoxMeshes;
	std::vector<BoundingBoxMesh*> m_SubBoundingBoxMeshes;
	
	int boundingcur = 0 ;
	int subboundingcur = 0 ;
	CapsuleMesh* m_pCapsuleMesh=NULL;
	ShphereMesh* m_psphere=NULL;

	CapsuleMesh* m_pPlayerCapsuleMesh=NULL;
	XMFLOAT3 m_PlayerCapsulePos;

	int EffectIndex[11] = { 0,1,2,5,8,9,10,11,12,13,16 };

	ResourceManager* m_pResourceManager = NULL;

	SoundManager* m_pSoundManager = NULL;
	LPDIRECTSOUNDBUFFER HittedSound = nullptr;
	LPDIRECTSOUNDBUFFER ShortExplosion = nullptr;
	LPDIRECTSOUNDBUFFER MonsterDie = nullptr;
	LPDIRECTSOUNDBUFFER MonsterDieEffect = nullptr;
	LPDIRECTSOUNDBUFFER DogDie = nullptr;
	LPDIRECTSOUNDBUFFER Teleport = nullptr;

public:

	CollisionManager(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, SoundManager* soundmanager);
	~CollisionManager();
	XMFLOAT4X4 MakeScaleMatrix(float x , float y , float z);
	XMFLOAT4X4 GetSphereMatrix(BSphere* pSphere);
	XMFLOAT4X4 GetSphereMatrix(BCapsule* pSphere); //ĸ���� ��
	XMFLOAT4X4 GetSphereMatrix(const float& r, const XMFLOAT3& center);
	//��� �ڽ�
	void EnrollObjectIntoBox(bool isAccel,XMFLOAT3 center, XMFLOAT3  extend, GameObject* pOwner);
	void EnrollObjectIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT4X4 Transform, GameObject* pOwner);
	void EnrollObjectIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT3 extendscale, GameObject* pOwner);
	void EnrollItemIntoBox(XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT4X4 Transform, GameObject* pOwner);



	void EnrollHierarchicalStaticGameObject(GameObject* pOwner);

	//��� ��
	void EnrollObjectIntoSphere(bool isAccel, XMFLOAT3 center,float radius, GameObject* pOwner);

	void EnrollObjectIntoCapsule(bool isAccel, XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner);

	void EnrollPlayerIntoCapsule(XMFLOAT3 center, float radius, float tip, float base, GameObject* pOwner);

	void EnrollEnemy(GameObject* pEnemy);
	void EnrollDogEnemy(GameObject* pEnemy);
	void EnrollBossEnemy(GameObject* pEnemy);


	void CheckCollisionEnemytoStaticObject(GameObject* pEnemy);
	//�Ѿ� ���

	void EnrollbulletIntoBox(bool isAccel, XMFLOAT3 center, XMFLOAT3 extend, XMFLOAT4X4 Transform, GameObject* pOwner);

	void LoadCollisionBoxInfo(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const char* filename);
	
	void EnrollBulletDir(Camera* pCamera) {m_pCamera = pCamera;}

	void EnrollRedZoneIntoSphere(XMFLOAT3 center, float radius, GameObject* pOwner);


	void CheckVisiableEnemy();


	//�浹 ó�� �Լ���
	bool CollisionPlayerToStaticObeject();
	bool CollisionPlayerToSubSceneStaticObeject();
	bool CollisionPlayerToItemBox();
	
	void RenderEnemyBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pObject);
	void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList);

	bool CollsionBulletToEnemy(vector<Billboard*>* m_pBloodBillboard, int& KillCount);
	bool CollsionBulletToEnemy(vector<Billboard*>* m_pBloodBillboard,GameObject* pEnemy,int& KillCount);

	bool CollsionBulletToDogEnemy(vector<Billboard*>& m_pBloodBillboard, int& KillCount);
	bool CollsionBulletToBossEnemy(vector<Billboard*>& m_pBloodBillboard, vector<Billboard*>& m_pCritcalBillboard, int& KilCount);

	void CollisionEnemyToStaticObeject();    
	void CollisionPlayerToEnemy();
	void CollisionEnemyToPlayer();
	void CollsiionBossToPlayer();
	bool CollisionPlayerToRedZone();
	void CollisionBulletToObject();



	void ReleaseUploadBuffers();

	void CollisionBulletToItemBox(vector<Billboard*>& ExplosionEffect, vector<Item*>& Items);
	
	void SetSamplingItem(Item* pGameObject, const XMFLOAT3& pos, int num);
	void SetSyringeItem(Item * pGameObject, const XMFLOAT3& pos);

	void SetShotgunItem(Item* pGameObject, const XMFLOAT3& pos);
	void SetMachinegunItem(Item* pGameObject, const XMFLOAT3& pos);
	void SetHealItem(Item* pGameObject, const XMFLOAT3& pos);
	void SetSpeedItem(Item* pGameObject, const XMFLOAT3& pos);
	void SetDamageItem(Item* pGameObject, const XMFLOAT3& pos);
	void SetSheildItem(Item* pGameObject, const XMFLOAT3& pos);

};

