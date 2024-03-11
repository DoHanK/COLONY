#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"
#include "CompositeGoal.h"

class AlienSpider;

class GoalExplore: public GoalComposite<AlienSpider>{


private:

    


public:

    GoalExplore(AlienSpider* pBot) :GoalComposite(pBot, Explore_Goal) {};

    void Activate();
    int  Process();
    void Terminate();

};

