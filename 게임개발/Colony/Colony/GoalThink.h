#pragma once
#include "stdafx.h"
#include "Goal.h"

class GoalEvalutor;
class GoalThink : GoalComposite{
private:
	std::vector<GoalEvalutor*>  m_GoalEvalutors;
public:
	GoalThink(GameObject* pOwner);
	~GoalThink();

	void Arbitrate();

	virtual void Activate();

	virtual void Terminate();

	virtual int  Process();

};

