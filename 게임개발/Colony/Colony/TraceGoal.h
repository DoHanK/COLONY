#pragma once
#include "AlienSpider.h"
#include "Goal.h"

class TraceGoal :public Goal<AlienSpider> {
public:
	TraceGoal(AlienSpider* pOwner);
	~TraceGoal();


	void Activate();
	int  Process();
	void Terminate();
};

