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
