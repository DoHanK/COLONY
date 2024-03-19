#include "WaitGoal.h"

WaitGoal::WaitGoal(AlienSpider* pOwner) : Goal(pOwner, Wait_Goal) {


}

WaitGoal::~WaitGoal()
{
}

void WaitGoal::Activate()
{
	
	m_time = 5.f;
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
