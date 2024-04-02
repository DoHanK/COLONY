#include "WanderForPlayerGoal.h"

void WanderForPlayerGoal::Activate()
{

	m_iStatus = active;
	AddSubgoal(new WanderAndWaitGoal(m_pOwner));


}

int WanderForPlayerGoal::Process()
{
	ActivateIfInactive();
	m_iStatus = ProcessSubGoals();

	if (m_pOwner->m_pPlayer) {

		if (m_SubGoals.front()->GetGoalType() == WanderAndWait_Goal) {
			RemoveAllSubgoals();
			AddSubgoal(new TraceGoal(m_pOwner));
		}
	}
	else {

		if (m_SubGoals.front()->GetGoalType() == Trace_Goal) {
			RemoveAllSubgoals();
			AddSubgoal(new WanderAndWaitGoal(m_pOwner));
		}

	}


	return m_iStatus;
}

void WanderForPlayerGoal::Terminate()
{


}