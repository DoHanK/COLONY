#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(AlienSpider* pOwner): Goal(pOwner,FollowPath_Goal){

}

FollowPathGoal::~FollowPathGoal()
{
}

void FollowPathGoal::Activate()
{
	m_iStatus = active;

	m_pOwner->m_Path = m_pOwner->m_pPathFinder->QueryPath(m_pOwner->GetPosition());
	m_pOwner->m_pRoute->Update(m_pOwner->m_Path);
	m_pOwner->m_pSoul->m_StartTravel = false;
}

int FollowPathGoal::Process(){

	ActivateIfInactive();	

	PushGoalType();

	if (m_pOwner->m_Path.size() == 0) {
		m_iStatus = completed;
		OutputDebugStringA("FollowPathGoal completed");
	}

	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
