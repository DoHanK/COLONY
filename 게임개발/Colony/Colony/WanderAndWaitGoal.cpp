#include "WanderAndWaitGoal.h"

WanderAndWaitGoal::WanderAndWaitGoal(AlienSpider* pOwner):CompositeGoal<AlienSpider>(pOwner, WanderAndWait_Goal)
{

}

WanderAndWaitGoal::~WanderAndWaitGoal()
{

}

void WanderAndWaitGoal::Activate()
{
	m_iStatus = active;
	AddSubgoal(new WanderGoal(m_pOwner));
}

int WanderAndWaitGoal::Process()
{
	PushGoalType();
	ActivateIfInactive();
	m_iStatus = ProcessSubGoals();

	if (m_SubGoals.front()->GetGoalType() == Wander_Goal) {
		if (m_pOwner->m_WaitCoolTime > 4.0f) {

			AddSubgoal(new WaitGoal(m_pOwner));
			m_pOwner->m_WaitCoolTime = 0;
		}

	}


	return m_iStatus;
}

void WanderAndWaitGoal::Terminate()
{
}
