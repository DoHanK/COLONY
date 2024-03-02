#include "Goal.h"



void Goal::ActivateIfInactive()
{
    if (isInactive())
    {
        Activate();
    }
}

void Goal::ReactivateIfFailed()
{
    if (hasFailed())
    {
        m_iStatus = inactive;
    }

}




int GoalComposite::ProcessSubGoals()
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

void GoalComposite::AddSubgoal(Goal* g)
{
    m_SubGoals.push_front(g);
}

void GoalComposite::RemoveAllSubgoals()
{
    for (std::list<Goal*>::iterator it = m_SubGoals.begin();
        it != m_SubGoals.end();
        ++it)
    {
        (*it)->Terminate();

        delete* it;
    }

    m_SubGoals.clear();
}
