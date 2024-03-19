#include "TraceGoal.h"

TraceGoal::TraceGoal(AlienSpider* pOwner):Goal<AlienSpider>(pOwner, Trace_Goal)
{
}

TraceGoal::~TraceGoal()
{
}

void TraceGoal::Activate()
{
}

int TraceGoal::Process()
{
	return 0;
}

void TraceGoal::Terminate()
{
}
