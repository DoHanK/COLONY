#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"
#include "CompositeGoal.h"
#include "ExploreGoal.h"
#include "TraceGoal.h"
#include "WanderAndWaitGoal.h"
#include "TraceAndAttackGoal.h"

class AlienSpider;

class WanderForPlayerGoal : public CompositeGoal<AlienSpider> {
public:

    WanderForPlayerGoal(AlienSpider* pBot) :CompositeGoal(pBot, Wander_Player_Goal) {};

    void Activate();
    int  Process();
    void Terminate();



};
