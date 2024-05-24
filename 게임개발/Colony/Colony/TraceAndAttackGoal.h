#pragma once
#include "stdafx.h"
#include "Goal.h"
#include "AlienSpider.h"
#include "CompositeGoal.h"
#include "UtilityGoal.h"

class AlienSpider;


class TraceAndAttack : public CompositeGoal<AlienSpider> {


private:




public:

    TraceAndAttack(AlienSpider* pBot) :CompositeGoal(pBot, TraceAndAttack_Goal) {};

    void Activate();
    int  Process();
    void Terminate();

};