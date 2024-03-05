#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(AlienSpider* pOwner): Goal(pOwner){

}

FollowPathGoal::~FollowPathGoal()
{
}

void FollowPathGoal::Activate(){
}

int FollowPathGoal::Process(){
	ActivateIfInactive();	

	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
