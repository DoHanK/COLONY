#include "ExploreForPlayerGoal.h"

void ExploreForPlayerGoal::Activate()
{

	m_iStatus = active;
	AddSubgoal(new ExploreGoal(m_pOwner));


}

int ExploreForPlayerGoal::Process()
{
	ActivateIfInactive();
	m_iStatus = ProcessSubGoals();

	if (m_pOwner->m_pPlayer) {

		if (m_SubGoals.front()->GetGoalType() == Explore_Goal) {
			RemoveAllSubgoals();
			AddSubgoal(new TraceAndAttack(m_pOwner));
		}
	}
	else {

		if (m_SubGoals.front()->GetGoalType() == TraceAndAttack_Goal) {
			RemoveAllSubgoals();
			AddSubgoal(new ExploreGoal(m_pOwner));
		}

	}


	return m_iStatus;
}

void ExploreForPlayerGoal::Terminate()
{

	RemoveAllSubgoals();
}

