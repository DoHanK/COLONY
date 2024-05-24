#pragma once
#include "CompositeGoal.h"
#include  "UtilityGoal.h"
#include "WanderGoal.h"
#include "AlienSpider.h"

class AlienSpider;

class WanderAndWaitGoal:public CompositeGoal<AlienSpider> {
public:
	WanderAndWaitGoal(AlienSpider* pOwner);
	~WanderAndWaitGoal();


	void Activate();
	int  Process();
	void Terminate();

};

