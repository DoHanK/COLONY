#pragma once
#pragma warning(disable:4786)

#include "stdafx.h"
#include "ColonyGameObject.h"
#include "AlienSpider.h"
#include "Goal.h"
#include "CompositeGoal.h"
#include "FollowPathGoal.h"
#include "WanderGoal.h"
#include "WanderAndWaitGoal.h"
#include "ExploreForPlayerGoal.h"
#include "WanderForPlayerGoal.h"

class AlienSpider;



class GoalEvalutor {
public:

	int m;

};

class GoalThink :public CompositeGoal<AlienSpider>{
private:
	std::vector<GoalEvalutor*>  m_GoalEvalutors;
public:
	GoalThink(AlienSpider* pOwner);
	virtual ~GoalThink();

	void Arbitrate();

	virtual void Activate();

	virtual void Terminate();

	virtual int  Process();

	void SetInactive() { m_iStatus = inactive; }

};

