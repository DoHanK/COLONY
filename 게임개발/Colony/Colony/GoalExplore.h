#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"

class GoalExplore: public Goal<AlienSpider>{


private:

    

    bool      m_bLastEdgeInPath;



public:

    GoalExplore(AlienSpider* pBot):Goal(pBot) {};

    void Activate();
    int  Process();
    void Terminate();

};

