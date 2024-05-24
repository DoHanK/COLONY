#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"
#include "CompositeGoal.h"
#include "ExploreGoal.h"
#include "TraceGoal.h"
#include "TraceAndAttackGoal.h"

class AlienSpider;

class ExploreForPlayerGoal : public CompositeGoal<AlienSpider> {
public:

    ExploreForPlayerGoal(AlienSpider* pBot) :CompositeGoal(pBot, ExploreForPlayer_Goal) {};

    void Activate();
    int  Process();
    void Terminate();



};

