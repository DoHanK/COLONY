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

void Player::SetAnimationFromInput(DWORD dwDir,DWORD dwState)
{


	//idle
	if (dwState & STATE_IDLE) {
		m_AnimationState = IDLE_RIFLE;
		m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_RIFLE);
		//m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_GUNPLAY);
		//m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_RELOAD);
		//m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_PICK_UP);
		//m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_JUMP);
		//m_pSkinnedAnimationController->SetTrackAnimationSet(0, IDLE_LANDING);

	}

	else if (dwState & STATE_WALK) {

		if (dwDir & DIR_FORWARD && (m_AnimationState != WALK_FORWARD)) {
			m_AnimationState = WALK_FORWARD;
			m_pSkinnedAnimationController->SetTrackAnimationSet(0, WALK_FORWARD);

		}
		if (dwDir & DIR_BACKWARD && (m_AnimationState != WALK_BACKWORD)){
			m_AnimationState = WALK_BACKWORD;
			m_pSkinnedAnimationController->SetTrackAnimationSet(0, WALK_BACKWORD);
		}
		if (dwDir & DIR_LEFT && (m_AnimationState != WALK_LEFT)) {
			m_AnimationState = WALK_LEFT;
			m_pSkinnedAnimationController->SetTrackAnimationSet(0, WALK_LEFT);

		}
		if (dwDir & DIR_RIGHT && (m_AnimationState != WALK_RIGHTWORD)) {
			m_AnimationState = WALK_RIGHTWORD;
			m_pSkinnedAnimationController->SetTrackAnimationSet(0, WALK_RIGHTWORD);

		}

	}

}
