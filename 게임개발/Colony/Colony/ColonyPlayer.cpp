#include "ColonyPlayer.h"

Player::Player()
{
	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

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

// 현재 속력= 속도 + 가속도 * 시간 
void Player::CalVelocityFromInput(DWORD dwDirection, float Velocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, Velocity);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -Velocity);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, Velocity);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -Velocity);
		//if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, Velocity);
		//if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -Velocity);

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
	m_pCamera->Move(xmf3Shift);
}

void Player::Animate(float fTimeElapsed)
{
	OnPrepareRender();

	if (m_pSkinnedAnimationController) ((PlayerAnimationController*)m_pSkinnedAnimationController)->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}




PlayerAnimationController::PlayerAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel):
AnimationController(pd3dDevice, pd3dCommandList,  nAnimationTracks, pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new AnimationTrack[nAnimationTracks];

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;

	m_ppAnimationSets = new AnimationSets * [m_nSkinnedMeshes];
	m_pnAnimatedBoneFrames = new int[m_nSkinnedMeshes];
	m_ppSkinnedMeshes = new SkinnedMesh * [m_nSkinnedMeshes];
	m_pppAnimatedBoneFrameCaches = new GameObject * *[m_nSkinnedMeshes];

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

		m_ppAnimationSets[i] = pModel->m_ppAnimationSets[i];
		m_ppAnimationSets[i]->AddRef();

		m_pnAnimatedBoneFrames[i] = pModel->m_pnAnimatedBoneFrames[i];

		m_pppAnimatedBoneFrameCaches[i] = new GameObject * [m_pnAnimatedBoneFrames[i]];
		for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
		{
			m_pppAnimatedBoneFrameCaches[i][j] = pModel->m_pppAnimatedBoneFrameCaches[i][j];
		}
	}

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}



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
					m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = Matrix4x4::Add(Matrix4x4::Scale(xmf4x4LowerTransform, 0.8f), Matrix4x4::Scale(xmf4x4UpperTransform, 0.2f));
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



	//점프일땐 무력화
	if ((!isSameLowerState(IDLE_JUMP)) && (!isSameLowerState(IDLE_JUMPING)) &&(!isSameLowerState(IDLE_LANDING))) {

		if (dwState & STATE_IDLE) {

			if ((dwState == (STATE_PICK_UP | STATE_IDLE)) && (!isSameState(IDLE_PICK_UP))) {
				ChangeAnimation(IDLE_PICK_UP);

			}
			else if ((dwState == (STATE_IDLE | STATE_JUMP)) && (!isSameLowerState(IDLE_JUMP))) {

				ChangeLowerAnimation(IDLE_JUMP);

			}
			else if (dwState == STATE_IDLE && (!isSameState(IDLE_RIFLE))) {
				ChangeAnimation(IDLE_RIFLE);
			}

		}
		else if (dwState & STATE_WALK) {


			if (STATE_PICK_UP == (dwState & STATE_PICK_UP)) {
				if ((!isSameUpperState(IDLE_PICK_UP))) {
					ChangeUpperAnimation(IDLE_PICK_UP);
				}
			}
			else if (!isSameUpperState(IDLE_RIFLE)) {
				ChangeUpperAnimation(IDLE_RIFLE);
			}

			//이중키 체크후 검사
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
		else if (dwState & STATE_RUN) {


			if (STATE_PICK_UP == (dwState & STATE_PICK_UP)) {
				if ((!isSameState(WALK_PICK_UP)))
					ChangeAnimation(WALK_PICK_UP);
			}
			else {



				//이중키
				if ((dwDir == (DIR_FORWARD | DIR_LEFT)) && (!isSameState(WALK_FORWORD_LEFT))) {
					ChangeAnimation(WALK_FORWORD_LEFT);
				}
				else if ((dwDir == (DIR_FORWARD | DIR_RIGHT)) && (!isSameState(WALK_FORWORD_RIGHT))) {
					ChangeAnimation(WALK_FORWORD_RIGHT);
				}
				else if ((dwDir == (DIR_BACKWARD | DIR_LEFT)) && (!isSameState(WALK_BACKWORD_LEFT))) {
					ChangeAnimation(WALK_BACKWORD_LEFT);
				}
				else if ((dwDir == (DIR_BACKWARD | DIR_RIGHT)) && (!isSameState(WALK_BACKWORD_RIGHT))) {
					ChangeAnimation(WALK_BACKWORD_RIGHT);
				}
				//단일키
				else if ((dwDir == DIR_LEFT) && (!isSameState(WALK_LEFT))) {
					ChangeAnimation(WALK_LEFT);
				}
				else if ((dwDir == DIR_RIGHT) && (!isSameState(WALK_RIGHT))) {
					ChangeAnimation(WALK_RIGHT);
				}
				else if ((dwDir == DIR_FORWARD) && (!isSameState(WALK_FORWORD))) {
					ChangeAnimation(WALK_FORWORD);
				}
				else if ((dwDir == DIR_BACKWARD) && (!isSameState(WALK_BACKWORD))) {
					ChangeAnimation(WALK_BACKWORD);
				}
			}
		}

	}
	else {
		if (isSameLowerState(IDLE_JUMP)) {
			

			if (isAnimationPlayProgress(false, IDLE_JUMP, 0.9)) {
				ChangeLowerAnimation(IDLE_JUMPING);
			}

		}
		if (isSameLowerState(IDLE_JUMPING)) {

			//아랫면과 충돌시 실행되도록 조건문 추가
			if (isAnimationPlayProgress(false, IDLE_JUMPING, 0.9)) {
				ChangeLowerAnimation(IDLE_LANDING);
			}
		}
		if (isSameLowerState(IDLE_LANDING)) {
			//아랫면과 충돌시 실행되도록 조건문 추가
			if (isAnimationPlayProgress(false, IDLE_LANDING, 0.9)) {
				ChangeAnimation(IDLE_RIFLE);
			}
		}

	}



	//PICK_UP 상체 속도 조절
	if (isSameUpperState(IDLE_PICK_UP)|| isSameUpperState(WALK_PICK_UP)) {

		if(isSameUpperState(IDLE_PICK_UP))
			SetTrackSpeed(NOW_UPPERTRACK, 12.0f);
		else
			SetTrackSpeed(NOW_UPPERTRACK, 1.3f);
	}
	//점프 이동 하체 속도
	else if (isSameLowerState(IDLE_JUMP)) {
		SetTrackSpeed(NOW_LOWERTRACK, 1.3f);
	}
	else if (isSameLowerState(IDLE_JUMPING)) {
		SetTrackSpeed(NOW_LOWERTRACK, 1.3f);
	}
	else if (isSameLowerState(IDLE_LANDING)) {
		SetTrackSpeed(NOW_LOWERTRACK, 1.5f);
	}
	//걷기와 이동속도 조절
	else if (STATE_RUN == (dwState & STATE_RUN)) {
		
			SetTrackSpeed(NOW_UPPERTRACK, 1.0f);
			SetTrackSpeed(NOW_LOWERTRACK, 1.0f);
		
	}
	else if (STATE_WALK == (dwState & STATE_WALK)) {
			SetTrackSpeed(NOW_UPPERTRACK, 0.5f);
			SetTrackSpeed(NOW_LOWERTRACK, 0.5f);
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
//상태 검사
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

bool PlayerAnimationController::isAnimationPlayProgress(bool top, DWORD dwState, float progress)
{
	if (top) {
		if (m_pAnimationTracks[NOW_UPPERTRACK].m_fPosition / m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength > progress) {
			return true;
		}
	}
	else {
		if (m_pAnimationTracks[NOW_LOWERTRACK].m_fPosition / m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength > progress) {
			return true;
		}
	}
	return false;
}


