#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "Goal.h"

class FollowPathGoal:public Goal{
public:
	FollowPathGoal(GameObject* pOwner);
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};

