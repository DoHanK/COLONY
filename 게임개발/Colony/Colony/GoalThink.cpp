#include "GoalThink.h"



GoalThink::GoalThink(AlienSpider* pOwner):GoalComposite<AlienSpider>(pOwner){


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
	
	
	


}

void GoalThink::Activate()
{


}

void GoalThink::Terminate()
{


}

int GoalThink::Process()
{


	return 0;
}
