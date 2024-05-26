#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(AlienSpider* pOwner): Goal(pOwner,FollowPath_Goal){

}

FollowPathGoal::~FollowPathGoal()
{
}

void FollowPathGoal::Activate()
{
	m_iStatus = active;
	
	XMFLOAT2  temp2Pos = m_pOwner->m_pPathFinder->QueryCloseRandomPos(m_pOwner->GetPosition());
	XMFLOAT3  temp3Pos = XMFLOAT3(temp2Pos.x, 0.0f, temp2Pos.y);
	m_pOwner->m_Path = m_pOwner->m_pPathFinder->QueryPath(temp3Pos);
	m_pOwner->m_pRoute->Update(m_pOwner->m_Path);
	m_pOwner->m_pSoul->m_StartTravel = false;
}

int FollowPathGoal::Process(){

	ActivateIfInactive();	

	PushGoalType();

	if (m_pOwner->m_Path.size() == 0) {
		m_iStatus = completed;
	}

	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
