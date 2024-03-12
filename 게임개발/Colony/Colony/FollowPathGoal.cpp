#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(AlienSpider* pOwner): Goal(pOwner,FollowPath_Goal){

}

FollowPathGoal::~FollowPathGoal()
{
}

void FollowPathGoal::Activate()
{
	m_pOwner->m_Path = m_pOwner->m_pPathFinder->QueryClosePath(m_pOwner->GetPosition());
	m_pOwner->m_pRoute->Update(m_pOwner->m_Path);
	m_pOwner->m_pSoul->m_StartTravel = false;
	m_iStatus = active;
}

int FollowPathGoal::Process(){

	ActivateIfInactive();	
	PushGoalType();
	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
