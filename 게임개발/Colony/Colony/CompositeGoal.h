#pragma once
#include "Goal.h"



template <typename entity_type>
class CompositeGoal : public Goal<entity_type> {
protected:
	std::list<Goal<entity_type>*>		m_SubGoals;

	int ProcessSubGoals();

public:

	CompositeGoal(entity_type* pOwner,int GetType) :Goal<entity_type>(pOwner,GetType) {}

	virtual ~CompositeGoal() { RemoveAllSubgoals(); }

	virtual void Activate() = 0;

	virtual void Terminate() = 0;

	virtual int  Process() = 0;


	void         AddSubgoal(Goal<entity_type>* g);

	//this method iterates through the subgoals and calls each one's Terminate
	//method before deleting the subgoal and removing it from the subgoal list
	void         RemoveAllSubgoals();
};



template <typename entity_type>
int CompositeGoal<entity_type>::ProcessSubGoals()
{
    while (!m_SubGoals.empty() &&
        (m_SubGoals.front()->isComplete() || m_SubGoals.front()->hasFailed()))
    {
        m_SubGoals.front()->Terminate();
        delete m_SubGoals.front();
        m_SubGoals.pop_front();
    }

    if (!m_SubGoals.empty())
    {
        int StatusOfSubGoals = m_SubGoals.front()->Process();

        if (StatusOfSubGoals == completed && m_SubGoals.size() > 1)
        {
            return active;
        }

        return StatusOfSubGoals;
    }

    else
    {
        return completed;
    }
}




template <typename entity_type>
void CompositeGoal<entity_type>::AddSubgoal(Goal<entity_type>* g)
{
    m_SubGoals.push_front(g);
}

template <typename entity_type>
void CompositeGoal<entity_type>::RemoveAllSubgoals()
{
    for (std::list<Goal<entity_type>*>::iterator it = m_SubGoals.begin();
        it != m_SubGoals.end();
        ++it)
    {
        (*it)->Terminate();

        delete* it;
    }

    m_SubGoals.clear();
}
