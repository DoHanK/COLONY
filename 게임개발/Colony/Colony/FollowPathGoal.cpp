#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(AlienSpider* pOwner): Goal(pOwner){

}

void FollowPathGoal::Activate(){
}

int FollowPathGoal::Process(){
	ActivateIfInactive();	

	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
