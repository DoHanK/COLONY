#pragma once
#include "AlienSpider.h"
#include "GoalThink.h"

class GoalThink;
class AlienSpider;
class AlienSpiderAnimationController;



class AIController{
public:
	GoalThink* m_pBrain;
	AlienSpider* m_pBody;
	AlienSpiderAnimationController* m_pAnimationControl;
public:
	AIController(GoalThink* pBrain, AlienSpider* pBody);
	~AIController() {};


	//이동관련
	XMFLOAT2  m_dest;
	XMFLOAT3  m_dir;
	bool	  m_StartTravel;
	float     m_BetweenAngle;
	XMFLOAT3	  m_CrossBetween;
	//Animation
	int		  m_AnimationName;
public:
	bool ExecuteGoal(float fTimeElapsed);
private:
	void ExecuteFollowPath(float fTimeElapsed);
	void ExecuteWait(float fTimeElapsed);
	void ExecuteWanderPath(float fTimeElapsed);
};

