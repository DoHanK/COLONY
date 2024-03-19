#include "ExploreGoal.h"
#include "FollowPathGoal.h"
#include "WaitGoal.h"
void ExploreGoal::Activate()
{

	m_iStatus = active;
	AddSubgoal(new FollowPathGoal(m_pOwner));
	

}

int ExploreGoal::Process()
{
	ActivateIfInactive();
	m_iStatus = ProcessSubGoals();
	if (m_SubGoals.front()->GetGoalType() == FollowPath_Goal) {
		if (m_pOwner->m_WaitCoolTime > m_pOwner->m_EndureLevel) {
	
			AddSubgoal(new WaitGoal(m_pOwner));
			m_pOwner->m_WaitCoolTime = 0;
		}

	}


	return m_iStatus;
}

void ExploreGoal::Terminate()
{

	
}
