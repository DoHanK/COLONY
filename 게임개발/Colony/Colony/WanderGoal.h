#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "CompositeGoal.h"
#include "AlienSpider.h"

class AlienSpider;

class WanderGoal : public  Goal<AlienSpider>{
public:
	WanderGoal(AlienSpider* pOwner);



	void Activate();
	int  Process();
	void Terminate();


};

