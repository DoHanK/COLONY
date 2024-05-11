#pragma once
#include "AlienSpider.h"
#include "Goal.h"

class WaitGoal :public Goal<AlienSpider> {

public:
	WaitGoal(AlienSpider* pOwner,float WaitingTime);
	~WaitGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};


class HittedGoal :public Goal<AlienSpider> {

public:
	HittedGoal(AlienSpider* pOwner, float WaitingTime);
	~HittedGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};