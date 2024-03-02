#include "GoalExplore.h"

void GoalExplore::Activate()
{

	m_iStatus = active;


}

int GoalExplore::Process()
{
	ActivateIfInactive();
	



	return 1;
}

void GoalExplore::Terminate()
{

	
}
