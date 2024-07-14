#include "ColonyPlayer.h"
#include "ColonyGameObject.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
//								Player class -> (Parrent is GameObject)								//
//																									//		
//PlayerAnimationController class -> Control Animation by UserInput (Parrent is AnimationController)//
//////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										Player Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
Player::Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManager)
{
	CLoadedModelInfo* PlayerModelInfo = pResourceManager->BringModelInfo("Model/JU_Mannequin.bin", "Model/Textures/PlayerTexture/");
	CLoadedModelInfo* UMP5ModelInfo = pResourceManager->BringModelInfo("Model/Weapon/rifle.bin", "Model/Textures/UMP5Texture/");
	CLoadedModelInfo* shotgunModelInfo = pResourceManager->BringModelInfo("Model/Weapon/shotgun.bin", "Model/Textures/Item/");
	CLoadedModelInfo* machinegunModelInfo = pResourceManager->BringModelInfo("Model/Weapon/machinegun.bin", "Model/Textures/Item/");


	if (PlayerModelInfo) {
		SetChild(PlayerModelInfo->m_pModelRootObject,true);
		PlayerModelInfo->m_pModelRootObject;
		m_RightHand = PlayerModelInfo->m_pModelRootObject->FindFrame("RightHand");
		m_LeftHand = PlayerModelInfo->m_pModelRootObject->FindFrame("LeftHand");
		m_Spine = PlayerModelInfo->m_pModelRootObject->FindFrame("Spine1");
	}
	SetAnimator(new PlayerAnimationController(pd3dDevice, pd3dCommandList, 4, PlayerModelInfo));

	m_UMP5Object = new GameObject();
	m_UMP5Object->SetChild(UMP5ModelInfo->m_pModelRootObject);
	m_shotgunObject = new GameObject();
	m_shotgunObject->SetChild(shotgunModelInfo->m_pModelRootObject);
	m_machinegunObject = new GameObject();
	m_machinegunObject->SetChild(machinegunModelInfo->m_pModelRootObject);
	
	// 플레이어 초기 Weapon 상태는 UMP5(rifle)
	ChangeRifle();

	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, -35.0f, 0.0f);
	m_fMaxVelocityXZ = 5.5f;
	m_fMaxVelocityY = 15.5f;
	m_fFriction = 17.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;


}

Player::~Player()
{
	if (m_pCamera) {
		m_pCamera->ReleaseShaderVariables();
		delete m_pCamera;
	}



}

float Player::GetShootingCoolTime()
{
	if (m_gunType == HAVE_RIFLE) {

		return 0.3f;
	}
	else if (m_gunType == HAVE_SHOTGUN) {

		return 0.15f;
	}
	else if (m_gunType == HAVE_MACHINEGUN) {

		return 0.1f;
	}
}

float Player::GetAmp()
{
	if (m_gunType == HAVE_RIFLE) {

		return 0.33f;
	}
	else if (m_gunType == HAVE_SHOTGUN) {

		return 0.8f;
	}
	else if (m_gunType == HAVE_MACHINEGUN) {

		return 0.1f;
	}
}

float Player::GetzRange(bool bStand)
{
	if (bStand) {

		if (m_gunType == HAVE_RIFLE) {

			return 0.33f;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return 1.5f;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return 0.05f;
		}

	}
	else {

		if (m_gunType == HAVE_RIFLE) {

			return 0.33f;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return 0.7f;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return 0.05f;
		}
	}
}

float Player::GetyRange(bool bStand)
{

	if (bStand) {

		if (m_gunType == HAVE_RIFLE) {

			return 0.33f;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return 1.0f;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return 0.005f;
		}

	}
	else {

		if (m_gunType == HAVE_RIFLE) {

			return 0.33f;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return 1.0f;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return 0.005f;
		}

	}
}

float Player::GetxRange(bool bStand)
{

	if (bStand) {

		if (m_gunType == HAVE_RIFLE) {

			return floatSignDis(gen) / 10;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return floatSignDis(gen) / 10;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return floatSignDis(gen) / 10;
		}

	}
	else {
		if (m_gunType == HAVE_RIFLE) {

			return floatSignDis(gen) / 10;
		}
		else if (m_gunType == HAVE_SHOTGUN) {

			return floatSignDis(gen) / 10;
		}
		else if (m_gunType == HAVE_MACHINEGUN) {

			return floatSignDis(gen) / 10;
		}
	}
}

void Player::SetAnimator(PlayerAnimationController* animator)
{
	GameObject::SetAnimator(animator);

	animator->m_player = this;
}

void Player::SetWeapon(GameObject* Weapon, int WeaponType)
{
	m_SelectWeapon.m_pChild = Weapon;
	m_SelectWeapon.m_pChild->m_xmf4x4World = Matrix4x4::Identity();
	m_SelectWeapon.m_pChild->m_xmf4x4ToParent = Matrix4x4::Identity();
	if(m_pExportEffect) m_pExportEffect->m_ownerObject = m_SelectWeapon.FindFrame("Export");
	if (WeaponType == HAVE_RIFLE) {
		//오른쪽 손
		XMFLOAT3 temp;
		temp = XMFLOAT3(0, 0, 1);
		m_SelectWeapon.m_pChild->Rotate(&temp, 100.f);
		temp = XMFLOAT3(0, 1, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, -8.f);
		temp = XMFLOAT3(1, 0, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, 8.f);

		m_SelectWeapon.m_pChild->MoveStrafe(0.35f);
		m_SelectWeapon.m_pChild->MoveUp(0.1f); // 0.1f
		m_SelectWeapon.m_pChild->MoveForward(0.04f);

		m_gunType = HAVE_RIFLE;
	}
	else if (WeaponType == HAVE_SHOTGUN) {
		XMFLOAT3 temp;
		temp = XMFLOAT3(0, 1, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, 110.f);
		temp = XMFLOAT3(1, 0, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, 90.f);
		temp = XMFLOAT3(0, 0, 1);
		m_SelectWeapon.m_pChild->Rotate(&temp, 180.f);
		temp = XMFLOAT3(0, 1, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, -10.f);

		m_SelectWeapon.m_pChild->MoveUp(0.08f); // 0.1f
		m_SelectWeapon.m_pChild->MoveForward(-0.3f);

		m_gunType = HAVE_SHOTGUN;
	} 
	else if (WeaponType == HAVE_MACHINEGUN) {
		XMFLOAT3 temp;
		temp = XMFLOAT3(0, 0, 1);
		m_SelectWeapon.m_pChild->Rotate(&temp, 160.f);
		temp = XMFLOAT3(0, 1, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, -110.f);
		temp = XMFLOAT3(1, 0, 0);
		m_SelectWeapon.m_pChild->Rotate(&temp, 15.f);
		temp = XMFLOAT3(0, 0, 1);
		m_SelectWeapon.m_pChild->Rotate(&temp, 5.f);

		m_SelectWeapon.m_pChild->MoveUp(-1.0f); // 0.1f
		m_SelectWeapon.m_pChild->MoveForward(-0.2f); // 0.1f



		m_gunType = HAVE_MACHINEGUN;
	
	}
		m_WeaponState = RIGHT_HAND;
		((PlayerAnimationController*)m_pSkinnedAnimationController)->m_WeaponState = RIGHT_HAND;
}

void Player::SetPosition(const XMFLOAT3& Position)
{
	m_xmf3Position = Position;
	GameObject::SetPosition(Position);
}
// 현재 속력= 속도 + 가속도 * 시간 
void Player::CalVelocityFromInput(DWORD dwDirection, float Acceleration, float fElapsedTime)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		float Dir = 1.0f;
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, Dir);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -Dir);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, Dir);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -Dir);

		float ScalarVelocity = Acceleration * fElapsedTime;

		//xmf3Shift = Vector3::Normalize(xmf3Shift);
		xmf3Shift = Vector3::ScalarProduct(xmf3Shift, ScalarVelocity);
		
			if (dwDirection & DIR_JUMP_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, 100000 * fElapsedTime);
	


		AddAccel(xmf3Shift);
	}
}
//가속도 증가
void Player::AddAccel(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
}
//해당 벡터만큼 이동
void Player::AddPosition(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}
//플레이어 위치 업데이트
void Player::UpdatePosition(float fTimeElapsed)
{
	
	 m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	//마찰계수
	float  fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;		

	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);

	}


	if (isJump) {
		float fMaxVelocityY = m_fMaxVelocityY;
		fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
		if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	}
	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	m_xmfPre3Position = m_xmf3Position;
	AddPosition(xmf3Velocity);
	m_xmfPre3Velocity = xmf3Velocity;

	m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	m_pCamera->RegenerateViewMatrix();

	//fLength = Vector3::Length(m_xmf3Velocity);
	//float fDeceleration = (m_fFriction * fTimeElapsed);
	//if (fDeceleration > fLength) fDeceleration = fLength;
	//m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

}

void Player::Rotate(float x, float y , float z)
{


		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +35.0f) { x -= (m_fPitch - 35.0f); m_fPitch = +35.0f; }
			if (m_fPitch < -35.0f) { x -= (m_fPitch + 35.0f); m_fPitch = -35.0f; }
			XMFLOAT3 DistanceByAngle = m_pCamera->GetOffset();
			if (m_fPitch > 0) {
				DistanceByAngle.y = DistanceByAngle.y + x / 40;
				DistanceByAngle.z = DistanceByAngle.z - x / 500;
			}
			else {
				DistanceByAngle.y = DistanceByAngle.y + x / 40;
				DistanceByAngle.z = DistanceByAngle.z - x / 150;
			}
		
			m_pCamera->SetOffset(DistanceByAngle);
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	

}

void Player::UpdateMatrix()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

int Player::GetBulletDamage()
{
	if (m_gunType == HAVE_SHOTGUN) {
		return 100;
	}
	else if (m_gunType == HAVE_RIFLE) {

		return 40;
	}
	else {
		return 30;
	}

	return 0;
}

void Player::SetCamera(ThirdPersonCamera* pCamera)
{
	if (m_pCamera) {
		m_pCamera->Release();
		m_pCamera = NULL;	
	}

	if (pCamera) {
		m_pCamera = pCamera;
		m_pCamera->AddRef();
	}
}

void Player::ChangeShotgun()
{
	SetWeapon(m_shotgunObject, HAVE_SHOTGUN);
}

void Player::ChangeRifle()
{
	SetWeapon(m_UMP5Object, HAVE_RIFLE);
}

void Player::Chagnemachinegun()
{
	
	SetWeapon(m_machinegunObject, HAVE_MACHINEGUN);
}

void Player::ReleaseUploadBuffers()
{
	GameObject::ReleaseUploadBuffers();
	m_UMP5Object->ReleaseUploadBuffers();
	m_shotgunObject->ReleaseUploadBuffers();
	m_machinegunObject->ReleaseUploadBuffers();
}

void Player::Animate(float fTimeElapsed)
{
	UpdateMatrix();

	if (m_pSkinnedAnimationController) ((PlayerAnimationController*)m_pSkinnedAnimationController)->AdvanceTime(fTimeElapsed, this);
	if (m_pSkinnedAnimationController) ((PlayerAnimationController*)m_pSkinnedAnimationController)->DirectUpdateMatrix();

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);

	if (m_WeaponState == RIGHT_HAND && m_RightHand) m_SelectWeapon.m_xmf4x4ToParent = m_RightHand->m_xmf4x4World;
	else if(m_WeaponState == SPINE_BACK && m_Spine) m_SelectWeapon.m_xmf4x4ToParent = m_Spine->m_xmf4x4World;
}

void Player::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//프레임으로 이뤄진 것이기에 필요가 없음
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i);

			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);

	m_SelectWeapon.UpdateTransform();
	m_SelectWeapon.Render(pd3dCommandList, pCamera);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//								PlayerAnimationController Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerAnimationController::PlayerAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel):
AnimationController(pd3dDevice, pd3dCommandList,  nAnimationTracks, pModel)
{

}

void PlayerAnimationController::AdvanceTime(float fElapsedTime, GameObject* pRootGameObject)
{
	m_fTime += fElapsedTime;

	//상체
	if (m_nowAnimationUpperWeight < 1.0f) {
		m_nowAnimationUpperWeight += fElapsedTime * 9 ;
		m_PreAnimationUpperWeight = 1.0f - m_nowAnimationUpperWeight;

	}
	else {
		m_nowAnimationUpperWeight = 1.0f;
		m_PreAnimationUpperWeight = 0.0f;
	}

	//하체
	if (m_nowAnimationLowerWeight < 1.0f) {
		m_nowAnimationLowerWeight += fElapsedTime * 12;
		m_PreAnimationLowerWeight = 1.0f - m_nowAnimationLowerWeight;

	}
	else {
		m_nowAnimationLowerWeight = 1.0f;
		m_PreAnimationLowerWeight = 0.0f;
	}
	//현재 상하체 가중치
	SetTrackWeight(NOW_UPPERTRACK, m_nowAnimationUpperWeight);
	SetTrackWeight(NOW_LOWERTRACK, m_nowAnimationLowerWeight);
	//이전 상하체 가중치
	SetTrackWeight(PRE_UPPERTRACK, m_PreAnimationUpperWeight);
	SetTrackWeight(PRE_LOWERTRACK, m_PreAnimationLowerWeight);

	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++) {
			//상체
			if (m_nowAnimationUpperWeight == 1 && (i == NOW_UPPERTRACK || i == PRE_UPPERTRACK)) {
				m_pAnimationTracks[i].m_fPosition += (fElapsedTime * m_pAnimationTracks[i].m_fSpeed);
			}

			//하체
			if (m_nowAnimationLowerWeight == 1 &&(i == NOW_LOWERTRACK || i == PRE_LOWERTRACK)) {
				m_pAnimationTracks[i].m_fPosition += (fElapsedTime * m_pAnimationTracks[i].m_fSpeed);
			}
		}
		for (int i = 0; i < m_nSkinnedMeshes; i++)
		{
			for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
			{
				XMFLOAT4X4 xmf4x4UpperTransform = Matrix4x4::Zero();
				XMFLOAT4X4 xmf4x4LowerTransform = Matrix4x4::Zero();

				for (int k = 0; k < m_nAnimationTracks; k++)
				{
					AnimationSet* pAnimationSet = m_ppAnimationSets[i]->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
					pAnimationSet->SetPosition(m_pAnimationTracks[k].m_fPosition);
					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
					//상체 계산
					if (k%2 == 0)
					{
						xmf4x4UpperTransform = Matrix4x4::Add(xmf4x4UpperTransform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));
					}
					else {
						xmf4x4LowerTransform = Matrix4x4::Add(xmf4x4LowerTransform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));
					}
				}

				//상체
				if (CheckLowerBody(string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName))) {
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = xmf4x4LowerTransform;
				}
				else {
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = xmf4x4UpperTransform;
				}

				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "Hips" ) {
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = Matrix4x4::Add(Matrix4x4::Scale(xmf4x4LowerTransform, 0.6f), Matrix4x4::Scale(xmf4x4UpperTransform, 0.4f));
				}
				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "Spine") {
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = Matrix4x4::Add(Matrix4x4::Scale(xmf4x4LowerTransform, 0.1f), Matrix4x4::Scale(xmf4x4UpperTransform, 0.9f));
				}
				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "RootBone") {
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = Matrix4x4::Add(Matrix4x4::Scale(xmf4x4LowerTransform, 0.5f), Matrix4x4::Scale(xmf4x4UpperTransform, 0.5f));
				}

				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "JU_Mannequin") {

					xmf4x4UpperTransform._41 = 0;
					xmf4x4UpperTransform._42 = 0;
					xmf4x4UpperTransform._43 = 0;
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = xmf4x4UpperTransform;
				}



			}
		}

		pRootGameObject->UpdateTransform(NULL);

	}

}

void PlayerAnimationController::SetAnimationFromInput(DWORD dwDir, DWORD dwState)
{

	if (m_player->isDance) {
		if (!isSameState(BREAK_DANCE)) {
			ChangeAnimation(BREAK_DANCE);
		}
	}
	else {
		/////////
		//상체
		////////
		//애니메이션을 바꿀수 있는 상태인지 체크
		if (IsUnChangeableUpperState() && IsUnChangeableLowerState()) {

			//총알 갈기, 총쏘기 , 줍기 ,
			if (dwState & STATE_RELOAD) {
				SetTrackSpeed(NOW_UPPERTRACK, 1.6f);
				ChangeUpperAnimation(IDLE_RELOAD);
			}
			else if (dwState & STATE_PICK_UP) {
				SetTrackSpeed(NOW_UPPERTRACK, 1.0f);
				ChangeUpperAnimation(WALK_PICK_UP);
			}
			else if (dwState & STATE_SWITCH_WEAPON) {

				SetTrackSpeed(NOW_UPPERTRACK, 4.0f);
				ChangeUpperAnimation(WEAPON_SWITCH_BACK);
				if (m_WeaponState == RIGHT_HAND) m_WeaponState = SPINE_BACK;
				else m_WeaponState = RIGHT_HAND;
			}
			else {

				if (dwState & STATE_IDLE) {

					if (m_WeaponState != SPINE_BACK) {

						if (!isSameUpperState(IDLE_RIFLE)) {
							ChangeUpperAnimation(IDLE_RIFLE);
						}
					}
					else {
						////상체
						if (!isSameUpperState(IDLE_NORMAL)) {
							ChangeUpperAnimation(IDLE_NORMAL);
						}
					}
				}
				else {
					//등에 총이 없을때
					if (m_WeaponState != SPINE_BACK) {

						if (!isSameUpperState(IDLE_RIFLE)) {
							ChangeUpperAnimation(IDLE_RIFLE);
						}

					}
					else {
						if (!isSameUpperState(RUNNING) && (dwDir & DIR_FORWARD)) {
							ChangeUpperAnimation(RUNNING);
						}

					}
				}
			}
		}
		else {

			if (isSameUpperState(IDLE_RELOAD)) {
				if (isAnimationPlayProgress(TRUE, IDLE_RELOAD, 0.9)) {
					SetTrackSpeed(NOW_UPPERTRACK, 1.0f);
					if (m_WeaponState == RIGHT_HAND) {
						ChangeUpperAnimation(IDLE_RIFLE);

					}
					else {
						ChangeUpperAnimation(IDLE_NORMAL);
					}
				}
			}
			else if (isSameUpperState(WALK_PICK_UP)) {
				if (isAnimationPlayProgress(TRUE, WALK_PICK_UP, 0.9)) {
					SetTrackSpeed(NOW_UPPERTRACK, 1.0f);
					if (m_WeaponState == RIGHT_HAND) {
						ChangeUpperAnimation(IDLE_RIFLE);

					}
					else {
						ChangeUpperAnimation(IDLE_NORMAL);
					}
				}
			}
			else if (isSameUpperState(WEAPON_SWITCH_BACK)) {

				if (isAnimationPlayProgress(TRUE, WEAPON_SWITCH_BACK, 0.3) && m_player->m_WeaponState != m_WeaponState) {
					//총의 계층을 바꿔주기
					if (m_player->m_WeaponState == RIGHT_HAND) {

						if (m_player->m_SelectWeapon.m_pChild) {

							

							m_player->m_SelectWeapon.m_pChild->m_xmf4x4World = Matrix4x4::Identity();
							m_player->m_SelectWeapon.m_pChild->m_xmf4x4ToParent = Matrix4x4::Identity();

							if (m_player->m_gunType == HAVE_RIFLE) {
								//오른쪽 손
								XMFLOAT3 temp;
								temp = XMFLOAT3(0, 0, 1);
								m_player->m_SelectWeapon.m_pChild->Rotate(&temp, -110.f);

								m_player->m_SelectWeapon.m_pChild->MoveStrafe(-0.3f);
								m_player->m_SelectWeapon.m_pChild->MoveForward(-0.3f);
							}
							else if (m_player->m_gunType == HAVE_SHOTGUN) {
								XMFLOAT3 temp;
								temp = XMFLOAT3(0, 1, 0);
								m_player->m_SelectWeapon.m_pChild->Rotate(&temp, 90.f);
								temp = XMFLOAT3(1, 0, 0);
								m_player->m_SelectWeapon.m_pChild->Rotate(&temp, -60.f);
								m_player->m_SelectWeapon.m_pChild->MoveStrafe(0.3f);
								m_player->m_SelectWeapon.m_pChild->MoveForward(0.7f);
							}
							else if (m_player->m_gunType == HAVE_MACHINEGUN) {


								XMFLOAT3 temp;
								temp = XMFLOAT3(0, 1, 0);
								m_player->m_SelectWeapon.m_pChild->Rotate(&temp, 100.f);
								temp = XMFLOAT3(1, 0, 0);
								m_player->m_SelectWeapon.m_pChild->Rotate(&temp, 30.f);

								m_player->m_SelectWeapon.m_pChild->MoveStrafe(0.3f);
								m_player->m_SelectWeapon.m_pChild->MoveForward(0.2f);

							}



						}
						m_player->m_WeaponState = SPINE_BACK;
					}
					else if (m_player->m_WeaponState == SPINE_BACK) {


						if (m_player->m_SelectWeapon.m_pChild) {
							m_player->m_SelectWeapon.m_pChild->m_xmf4x4World = Matrix4x4::Identity();
							m_player->m_SelectWeapon.m_pChild->m_xmf4x4ToParent = Matrix4x4::Identity();

							if (m_player->m_gunType == HAVE_RIFLE) {
								m_player->ChangeRifle();
							}
							else if (m_player->m_gunType == HAVE_SHOTGUN) {
								m_player->ChangeShotgun();

							}
							else if (m_player->m_gunType == HAVE_MACHINEGUN) {
								m_player->Chagnemachinegun();
							}


						}
						m_player->m_WeaponState = RIGHT_HAND;
					}
				}

				if (isAnimationPlayProgress(TRUE, WEAPON_SWITCH_BACK, 0.9)) {
					SetTrackSpeed(NOW_UPPERTRACK, 1.0f);

					if (m_WeaponState == RIGHT_HAND) {
						ChangeUpperAnimation(IDLE_RIFLE);

					}
					else {
						ChangeUpperAnimation(IDLE_NORMAL);
					}
				}


			}
		}


		/////////
		//하체
		////////
		//애니메이션을 바꿀수 있는 상태인지 체크
		if (IsUnChangeableLowerState()) {
			//가만히 서있을때
			if (dwState & STATE_IDLE) {

				if (dwState == STATE_IDLE && (!isSameLowerState(IDLE_RIFLE))) {
					ChangeLowerAnimation(IDLE_RIFLE);
				}
				else if ((dwState & STATE_JUMP)) {

					if (m_WeaponState != SPINE_BACK && (!isSameLowerState(WALK_JUMP))) {
						ChangeAnimation(WALK_JUMP);

					}
					else if (!isSameLowerState(IDLE_JUMP)) {
						SetTrackSpeed(NOW_LOWERTRACK, 2.0f);
						SetTrackSpeed(NOW_UPPERTRACK, 2.0f);
						ChangeAnimation(IDLE_JUMP);
					}

				}

			}
			//걸을때
			if (dwState & STATE_WALK || dwState & STATE_RUN) {

				//총이 있을때
				if (m_WeaponState != SPINE_BACK) {
					if ((dwState & STATE_JUMP) && (!isSameLowerState(WALK_JUMP))) {

						ChangeAnimation(WALK_JUMP);
					}
					else {

						if ((dwDir == (DIR_FORWARD | DIR_LEFT)) && (!isSameLowerState(WALK_FORWORD_LEFT))) {
							ChangeLowerAnimation(WALK_FORWORD_LEFT);
						}
						else if ((dwDir == (DIR_FORWARD | DIR_RIGHT)) && (!isSameLowerState(WALK_FORWORD_RIGHT))) {
							ChangeLowerAnimation(WALK_FORWORD_RIGHT);
						}
						else if ((dwDir == (DIR_BACKWARD | DIR_LEFT)) && (!isSameLowerState(WALK_BACKWORD_LEFT))) {
							ChangeLowerAnimation(WALK_BACKWORD_LEFT);
						}
						else if ((dwDir == (DIR_BACKWARD | DIR_RIGHT)) && (!isSameLowerState(WALK_BACKWORD_RIGHT))) {
							ChangeLowerAnimation(WALK_BACKWORD_RIGHT);
						}
						else if ((dwDir == DIR_LEFT) && (!isSameLowerState(WALK_LEFT))) {
							ChangeLowerAnimation(WALK_LEFT);
						}
						else if ((dwDir == DIR_RIGHT) && (!isSameLowerState(WALK_RIGHT))) {
							ChangeLowerAnimation(WALK_RIGHT);
						}
						else if ((dwDir == DIR_FORWARD) && (!isSameLowerState(WALK_FORWORD))) {
							ChangeLowerAnimation(WALK_FORWORD);
						}
						else if ((dwDir == DIR_BACKWARD) && (!isSameLowerState(WALK_BACKWORD))) {
							ChangeLowerAnimation(WALK_BACKWORD);
						}
					}
				}
				//총이 없을때 전력 질주
				else {

					if ((dwState & STATE_JUMP) && !(dwState & STATE_IDLE) && (!isSameLowerState(RUNNING_JUMP))) {
						SetTrackSpeed(NOW_LOWERTRACK, 1.3f);
						ChangeAnimation(RUNNING_JUMP);
					}
					else if ((!isSameLowerState(RUNNING)) && (dwDir & DIR_FORWARD)) {
						ChangeLowerAnimation(RUNNING);
					}//점프

				}

			}

		}
		else {

			//애니메이션 종료 체크
			if (isSameLowerState(IDLE_JUMP)) {
				//아랫면과 충돌시 실행되도록 조건문 추가
				if (isAnimationPlayProgress(false, IDLE_JUMP, 0.8)) {

					ChangeLowerAnimation(WALK_FORWORD);
				}
			}

			//애니메이션 종료 체크
			else if (isSameLowerState(WALK_JUMP)) {
				//아랫면과 충돌시 실행되도록 조건문 추가
				if (isAnimationPlayProgress(false, WALK_JUMP, 1.0)) {
					ChangeLowerAnimation(WALK_FORWORD);
				}
			}

			else if (isSameLowerState(RUNNING_JUMP)) {
				if (isAnimationPlayProgress(false, RUNNING_JUMP, 1.0)) {
					ChangeLowerAnimation(WALK_FORWORD);
				}
			}


		}


		//하체 걷기와 이동속도 조절
		if (STATE_RUN == (dwState & STATE_RUN)) {

			if (isSameLowerState(WALK_JUMP)) {
				SetTrackSpeed(NOW_LOWERTRACK, 1.4f);
			}
			else {

				SetTrackSpeed(NOW_LOWERTRACK, 1.5f);

				if (m_WeaponState == SPINE_BACK && IsUnChangeableUpperState()) {
					SetTrackSpeed(NOW_UPPERTRACK, 1.8f);
					SetTrackSpeed(NOW_LOWERTRACK, 1.8f);
				}

			}
		}
		else if (STATE_WALK == (dwState & STATE_WALK)) {

			if (isSameLowerState(WALK_JUMP)) {
				SetTrackSpeed(NOW_LOWERTRACK, 1.0f);
			}
			else {

				SetTrackSpeed(NOW_LOWERTRACK, 1.4f);
				//등에 있을때
				if (m_WeaponState == SPINE_BACK && IsUnChangeableUpperState()) {
					SetTrackSpeed(NOW_UPPERTRACK, 1.6f);
					SetTrackSpeed(NOW_LOWERTRACK, 1.6f);
				}

			}
		}
		else {


			if (!isSameState(IDLE_RIFLE) && !isSameState(IDLE_JUMP) && IsUnChangeableUpperState()) {
				SetTrackSpeed(NOW_UPPERTRACK, 1.0f);
				SetTrackSpeed(NOW_LOWERTRACK, 1.0f);
			}
		}

	}

}
//상체 하체 애니메이션 변경
void PlayerAnimationController::ChangeAnimation(DWORD ChangeState)
{
	ChangeUpperAnimation(ChangeState);
	ChangeLowerAnimation(ChangeState);
}
//상체 애니메이션 변경
void PlayerAnimationController::ChangeUpperAnimation(DWORD ChangeState)
{
	m_nowAnimationUpperWeight = 0.0f;

	SetTrackAnimationSet(PRE_UPPERTRACK, m_AnimationUpperState);

	m_AnimationUpperState = ChangeState;
	SetTrackAnimationSet(NOW_UPPERTRACK, ChangeState);


	m_pAnimationTracks[PRE_UPPERTRACK].m_fPosition = m_pAnimationTracks[NOW_UPPERTRACK].m_fPosition;
	m_pAnimationTracks[NOW_UPPERTRACK].m_fPosition = 0.0f;
}
//하체 애니메이션 변경
void PlayerAnimationController::ChangeLowerAnimation(DWORD ChangeState)
{

	m_nowAnimationLowerWeight = 0.0f;

	SetTrackAnimationSet(PRE_LOWERTRACK, m_AnimationLowerState);

	m_AnimationLowerState = ChangeState;
	SetTrackAnimationSet(NOW_LOWERTRACK, ChangeState);

	m_pAnimationTracks[PRE_LOWERTRACK].m_fPosition = m_pAnimationTracks[NOW_LOWERTRACK].m_fPosition;
	m_pAnimationTracks[NOW_LOWERTRACK].m_fPosition = 0.0f;
}
//하체프레임 검사
bool PlayerAnimationController::CheckLowerBody(std::string FrameName)
{

	if (FrameName == "LeftUpLeg")
		return true;
	if (FrameName == "LeftLeg")
		return true;
	if (FrameName == "LeftFoot")
		return true;
	if (FrameName == "LeftToeBase")
		return true;
	if (FrameName == "RightUpLeg")
		return true;
	if (FrameName == "RightLeg")
		return true;
	if (FrameName == "RightFoot")
		return true;
	if (FrameName == "RightToeBase")
		return true;


	return false;
}
//상하체 상태 검사
bool PlayerAnimationController::isSameState(DWORD dwState)
{
	if (isSameUpperState(dwState) && isSameLowerState(dwState))
		return true;
	return false;
}
//상체 검사
bool PlayerAnimationController::isSameUpperState(DWORD dwState)
{
	if (m_AnimationUpperState == dwState) return true;

	return false;
}
//하체 검사
bool PlayerAnimationController::isSameLowerState(DWORD dwState)
{
	if (m_AnimationLowerState == dwState) return true;

	return false;
}

bool PlayerAnimationController::IsUnChangeableUpperState()
{
	if ((!isSameUpperState(IDLE_PICK_UP)) && (!isSameUpperState(IDLE_RELOAD)) && (!isSameUpperState(WALK_RELOAD)) && (!isSameUpperState(WALK_PICK_UP)) && 
		(!isSameUpperState(WEAPON_SWITCH_BACK))
		) {
		return true;
	}
	return false;
}

bool PlayerAnimationController::IsUnChangeableLowerState()
{
	if ((!isSameLowerState(IDLE_JUMP)) && (!isSameLowerState(RUNNING_JUMP)) && (!isSameLowerState(WALK_JUMP))&& !isSameUpperState(WEAPON_SWITCH_BACK)) {
		return true;
	}
	return false;
}

bool PlayerAnimationController::isAnimationPlayProgress(bool top, DWORD dwState, float progress)
{
	if (top) {
		if (m_pAnimationTracks[NOW_UPPERTRACK].m_fPosition / m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength >= progress) {
			return true;
		}
	}
	else {
		if (m_pAnimationTracks[NOW_LOWERTRACK].m_fPosition / m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength >= progress) {
			return true;
		}
	}
	return false;
}




