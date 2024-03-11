#include "GoalThink.h"



GoalThink::GoalThink(AlienSpider* pOwner):GoalComposite<AlienSpider>(pOwner,Think_Goal){


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
	AddSubgoal(new GoalExplore(m_pOwner));

}

void GoalThink::Activate()
{
	Arbitrate();
	m_iStatus = active;
}



int GoalThink::Process()
{
	ActivateIfInactive();

	int SubgoalStatus = ProcessSubGoals();

	if (SubgoalStatus == completed || SubgoalStatus == failed)
	{
	
		m_iStatus = inactive;
		
	}

	return m_iStatus;
}

void GoalThink::Terminate()
{


}