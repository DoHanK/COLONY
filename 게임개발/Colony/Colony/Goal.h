#pragma once
#include "stdafx.h"

template <typename entity_type>
class Goal{
public:

	enum{active, inactive , completed , failed};

protected:

	int m_iStatus;
	int m_GoalType;
	entity_type* m_pOwner = NULL;

	void ActivateIfInactive();

	void ReactivateIfFailed();
public:
	Goal(entity_type* pOwner, int GoalType) : m_GoalType(GoalType), m_pOwner(pOwner), m_iStatus(inactive) {  };

	virtual ~Goal() {};

	virtual void Activate() = 0;

	virtual int Process() = 0;

	virtual void Terminate() = 0;

	void		 PushGoalType() { m_pOwner->m_GoalType = m_GoalType; };
	bool         isComplete()const { return m_iStatus == completed; }
	bool         isActive()const { return m_iStatus == active; }
	bool         isInactive()const { return m_iStatus == inactive; }
	bool         hasFailed()const { return m_iStatus == failed; }
	int			 GetGoalType()const { return m_GoalType; }


};



template <typename entity_type>
void Goal<entity_type>::ActivateIfInactive()
{
	if (isInactive())
	{
		Activate();
	}
}
template <typename entity_type>
void Goal<entity_type>::ReactivateIfFailed()
{
	if (hasFailed())
	{
		m_iStatus = inactive;
	}

}


