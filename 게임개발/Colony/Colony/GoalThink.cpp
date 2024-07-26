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


	

	if (m_SubGoals.size() > 0) {
		//타격 받았을때
		if (m_pOwner->m_bHitted) {
			AddSubgoal(new HittedGoal(m_pOwner, 0.5f));
			m_pOwner->m_bHitted = false;
		}

		// 죽었을때
		else if (m_pOwner->m_HP < 1 && m_SubGoals.front()->GetGoalType() != Deaded_Goal) {
			AddSubgoal(new DeadedGoal(m_pOwner, 3.0f));
		}



		if (m_NeedJump && m_SubGoals.front()->GetGoalType() != Deaded_Goal
			&& m_SubGoals.front()->GetGoalType() != Jump_Goal
			&& m_bJump == false
			&& m_pOwner->m_StuckTime > 2.0f)
		{
			m_pOwner->m_WaitCoolTime = 0;
			m_pOwner->m_pSoul->m_dest = m_pOwner->m_pPathFinder->QueryCloseRandomPos(m_pOwner->GetPosition());
			AddSubgoal(new JumpGoal(m_pOwner, 0.0f));
			m_NeedJump = false;
		}


	}



	if (SubgoalStatus == completed || SubgoalStatus == failed){
	
		m_iStatus = inactive;
		
	}

	return m_iStatus;
}

void GoalThink::Terminate()
{


}





DogGoalThink::DogGoalThink(GameObject* pOwner) :CompositeGoal<GameObject>(pOwner, Think_Goal) {


}

DogGoalThink::~DogGoalThink()
{
	std::vector<GoalEvalutor*>::iterator curDes = m_GoalEvalutors.begin();
	for (curDes; curDes != m_GoalEvalutors.end(); ++curDes)
	{
		delete* curDes;
	}

}

void DogGoalThink::Arbitrate()
{






}

void DogGoalThink::Activate()
{
	Arbitrate();
	m_iStatus = active;
}

int DogGoalThink::Process()
{
	ActivateIfInactive();
	static int a = 0;
	int SubgoalStatus = ProcessSubGoals();

	float distance = XM3CalDis(m_pOwner->m_pEnemy->GetPosition(), m_pOwner->GetPosition());
	if (m_pOwner->m_HP <= 0) {
		m_pOwner->m_GoalType = Deaded_Goal;
	}
	else{

		if (m_pOwner->m_bHitted) {
			m_pOwner->m_GoalType = Hitted_Goal;
		}
		else {

			if (distance < 20.0f) {

				if (distance < 1.0f) {
					m_pOwner->m_GoalType = Attack_Goal;
				}
				else if (m_pOwner->m_GoalType != Attack_Goal) {
					m_pOwner->m_GoalType = Trace_Goal;
				}

			}
			else {
				m_pOwner->m_GoalType = Idle_Goal;
			}
		}

	}


	if (SubgoalStatus == completed || SubgoalStatus == failed) {

		m_iStatus = inactive;

	}

	return m_iStatus;
}

void DogGoalThink::Terminate()
{


}