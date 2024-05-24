#include "GoalThink.h"



GoalThink::GoalThink(AlienSpider* pOwner):CompositeGoal<AlienSpider>(pOwner,Think_Goal){


}

GoalThink::~GoalThink()
{
	std::vector<GoalEvalutor*>::iterator curDes = m_GoalEvalutors.begin();
	for (curDes; curDes != m_GoalEvalutors.end(); ++curDes)
	{
		delete* curDes;
	}

}

void GoalThink::Arbitrate()
{
	if (IdleRandom(gen) == 0|| IdleRandom(gen) == 1|| IdleRandom(gen) == 2) {
		AddSubgoal(new ExploreForPlayerGoal(m_pOwner));
	}
	else {
		AddSubgoal(new WanderForPlayerGoal(m_pOwner));
	}


}

void GoalThink::Activate()
{
	Arbitrate();
	m_iStatus = active;
}



int GoalThink::Process()
{
	ActivateIfInactive();
	static int a = 0;
	int SubgoalStatus = ProcessSubGoals();



	//타격 받았을때
	if (m_pOwner->m_bHitted) {
		AddSubgoal(new HittedGoal(m_pOwner,0.5f));
		m_pOwner->m_bHitted = false;
	}

	// 죽었을때
	else if (m_pOwner->m_HP < 1 && m_SubGoals.front()->GetGoalType() != Deaded_Goal) {
		AddSubgoal(new DeadedGoal(m_pOwner, 3.0f));
	}





	if (SubgoalStatus == completed || SubgoalStatus == failed){
	
		m_iStatus = inactive;
		
	}

	return m_iStatus;
}

void GoalThink::Terminate()
{


}