#include "WaitGoal.h"

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
