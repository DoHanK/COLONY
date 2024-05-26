#include "UtilityGoal.h"

WaitGoal::WaitGoal(AlienSpider* pOwner , float WaitingTime) : Goal(pOwner, Wait_Goal) ,m_time(WaitingTime) {


}

WaitGoal::~WaitGoal()
{
}

void WaitGoal::Activate()
{
	
	m_pOwner->m_WaitingTime = 0.f;
	m_iStatus = active;
}

int WaitGoal::Process()
{
	ActivateIfInactive();
	PushGoalType();

	if (m_time < m_pOwner->m_WaitingTime) {
		m_iStatus = completed;
	}

	return m_iStatus;
}

void WaitGoal::Terminate()
{
	
}

HittedGoal::HittedGoal(AlienSpider* pOwner, float WaitingTime) : Goal(pOwner, Hitted_Goal), m_time(WaitingTime)
{

}

HittedGoal::~HittedGoal()
{
}

void HittedGoal::Activate()
{


	m_pOwner->m_WaitingTime = 0.f;
	m_iStatus = active;
}

int HittedGoal::Process()
{
	ActivateIfInactive();
	PushGoalType();
	if (m_time < m_pOwner->m_WaitingTime) {
		m_iStatus = completed;
	}

	return m_iStatus;
}

void HittedGoal::Terminate()
{

	m_pOwner->m_pSkinnedAnimationController->SetTrackSpeed(NOW_TRACK, 1.0f);
	m_pOwner->m_pSkinnedAnimationController->SetTrackSpeed(PRE_TRACK, 1.0f);
}

DeadedGoal::DeadedGoal(AlienSpider* pOwner, float WaitingTime) : Goal(pOwner, Deaded_Goal), m_time(WaitingTime)
{

}

DeadedGoal::~DeadedGoal()
{
}

void DeadedGoal::Activate()
{
	m_pOwner->m_WaitingTime = 0.f;
	m_iStatus = active;
}

int DeadedGoal::Process()
{
	ActivateIfInactive();
	PushGoalType();

	if (m_time < m_pOwner->m_WaitingTime) {

		m_iStatus = completed;
	}

	return m_iStatus;
}

void DeadedGoal::Terminate()
{
	m_pOwner->m_bActive = false;
	m_pOwner->m_pSkinnedAnimationController->SetTrackSpeed(NOW_TRACK, 1.0f);
	m_pOwner->m_pSkinnedAnimationController->SetTrackSpeed(PRE_TRACK, 1.0f);
}


AttackGoal::AttackGoal(AlienSpider* pOwner, float WaitingTime) : Goal(pOwner, Attack_Goal), m_time(WaitingTime)
{

}

AttackGoal::~AttackGoal()
{

}

void AttackGoal::Activate()
{
	m_pOwner->m_WaitingTime = 0.f;
	m_iStatus = active;
}

int AttackGoal::Process()
{
	ActivateIfInactive();
	PushGoalType();

	if (m_time < m_pOwner->m_WaitingTime) {

		m_iStatus = completed;
	}

	return m_iStatus;
}

void AttackGoal::Terminate()
{

}


JumpGoal::JumpGoal(AlienSpider* pOwner, float WaitingTime) : Goal(pOwner, Jump_Goal), m_time(WaitingTime) {


}

JumpGoal::~JumpGoal()
{
}

void JumpGoal::Activate()
{
	m_pOwner->m_WaitingTime = 0.f;
	m_iStatus = active;
	m_pOwner->m_pSoul->m_JumpStep = 0;
	m_pOwner->m_xmf3Velocity.y += 10.f;
	m_pOwner->m_pBrain->m_bJump = true;

}

int JumpGoal::Process()
{
	ActivateIfInactive();
	PushGoalType();

	if (m_pOwner->m_pPlayer) {//Player¹ß°ß

		XMFLOAT3 PlayerPos = m_pOwner->m_pPlayer->GetPosition();

		m_pOwner->m_pSoul->m_dest = XMFLOAT2(PlayerPos.x, PlayerPos.z);

	} 
	else {
		m_iStatus = completed;
	}

	if (m_pOwner->m_pSoul->m_JumpStep == JUMP_END) {
		m_iStatus = completed;
	}
	return m_iStatus;
}

 
void JumpGoal::Terminate()
{
	m_pOwner->m_pBrain->m_bJump = false;
}


