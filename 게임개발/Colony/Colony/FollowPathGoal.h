#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "Goal.h"
#include "AlienSpider.h"

class AlienSpider;


class FollowPathGoal:public Goal<AlienSpider>{

public:
	FollowPathGoal(AlienSpider* pOwner);
	~FollowPathGoal();
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};

