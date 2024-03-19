#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"
#include "CompositeGoal.h"

class AlienSpider;

class ExploreGoal: public CompositeGoal<AlienSpider>{


private:

    


public:

    ExploreGoal(AlienSpider* pBot) :CompositeGoal(pBot, Explore_Goal) {};

    void Activate();
    int  Process();
    void Terminate();

};

