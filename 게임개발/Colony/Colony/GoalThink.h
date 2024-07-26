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
#include "UtilityGoal.h"
class AlienSpider;



class GoalEvalutor {
public:

	int m;

};

class GoalThink :public CompositeGoal<AlienSpider>{
private:
	std::vector<GoalEvalutor*>  m_GoalEvalutors;
public:
	bool		m_NeedJump = false;
	bool		m_bJump = false;

	GoalThink(AlienSpider* pOwner);
	virtual ~GoalThink();

	void Arbitrate();

	virtual void Activate();

	virtual void Terminate();

	virtual int  Process();

	void SetInactive() { m_iStatus = inactive; }

};

class DogGoalThink :public CompositeGoal<GameObject> {
private:
	std::vector<GoalEvalutor*>  m_GoalEvalutors;
public:


	DogGoalThink(GameObject* pOwner);
	virtual ~DogGoalThink();

	void Arbitrate();

	virtual void Activate();

	virtual void Terminate();

	virtual int  Process();

	void SetInactive() { m_iStatus = inactive; }

};



class BossGoalThink :public CompositeGoal<GameObject> {
private:
	std::vector<GoalEvalutor*>  m_GoalEvalutors;
public:


	BossGoalThink(GameObject* pOwner);
	virtual ~BossGoalThink();

	void Arbitrate();

	virtual void Activate();

	virtual void Terminate();

	virtual int  Process();

	void SetInactive() { m_iStatus = inactive; }

};