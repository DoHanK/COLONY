#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "AlienSpider.h"

class Goal{
public:

	enum{active, inactive , completed , failed};

protected:

	int m_iStatus;

	AlienSpider* m_pOwner = NULL;

	void ActivateIfInactive();

	void ReactivateIfFailed();
public:
	Goal(AlienSpider* pOwner) :m_pOwner(pOwner) {};

	virtual ~Goal() {}

	virtual void Activate() = 0;

	virtual int Process() = 0;

	virtual void Terminate() = 0;

	bool         isComplete()const { return m_iStatus == completed; }
	bool         isActive()const { return m_iStatus == active; }
	bool         isInactive()const { return m_iStatus == inactive; }
	bool         hasFailed()const { return m_iStatus == failed; }



};


class GoalComposite : public Goal {
protected:
	std::list<Goal*>		m_SubGoals;

	int ProcessSubGoals();


public:

	GoalComposite(AlienSpider* pOwner) :Goal(pOwner) {};
	
	virtual ~GoalComposite() { RemoveAllSubgoals(); }

	virtual void Activate() = 0;

	virtual void Terminate() = 0;

	virtual int  Process() = 0;


	void         AddSubgoal(Goal* g);

	//this method iterates through the subgoals and calls each one's Terminate
	//method before deleting the subgoal and removing it from the subgoal list
	void         RemoveAllSubgoals();
};