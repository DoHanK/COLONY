#include "WanderGoal.h"

WanderGoal::WanderGoal(AlienSpider* pOwner): Goal(pOwner, Wander_Goal){


}

void WanderGoal::Activate(){

	m_iStatus = active;

	m_pOwner->m_Path = m_pOwner->m_pPathFinder->QueryClosePath(m_pOwner->GetPosition());
	m_pOwner->m_pRoute->Update(m_pOwner->m_Path);
	m_pOwner->m_pSoul->m_StartTravel = false;

}

int WanderGoal::Process(){

	ActivateIfInactive();

	PushGoalType();

	if (m_pOwner->m_Path.size() == 0) {
		m_iStatus = completed;
	}

	return m_iStatus;
	
}

void WanderGoal::Terminate(){


}
