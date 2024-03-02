#include "FollowPathGoal.h"

FollowPathGoal::FollowPathGoal(GameObject* pOwner): Goal(pOwner){

}

void FollowPathGoal::Activate(){
}

int FollowPathGoal::Process(){
	ActivateIfInactive();	

	return m_iStatus;
}

void FollowPathGoal::Terminate(){



}
