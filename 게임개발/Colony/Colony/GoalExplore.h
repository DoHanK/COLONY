#pragma once
#include "stdafx.h"
#include "Goal.h"
class GoalExplore: public Goal{


private:

    

    bool      m_bLastEdgeInPath;



public:

    GoalExplore(GameObject* pBot):Goal(pBot) {};

    void Activate();
    int  Process();
    void Terminate();

};

