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
	XMFLOAT3 m_xmf3Gravity = XMFLOAT3(0.0f, -9.8f, 0.0f);

	//이동관련
	XMFLOAT2  m_dest;
	XMFLOAT3  m_dir;
	bool	  m_StartTravel;
	float     m_BetweenAngle;
	XMFLOAT3	  m_CrossBetween;
	//Animation
	int		  m_AnimationName;
	int			m_JumpStep;
public:
	bool ExecuteGoal(float fTimeElapsed);
private:
	void ExecuteFollowPath(float fTimeElapsed);
	void ExecuteWait(float fTimeElapsed);
	void ExecuteWanderPath(float fTimeElapsed);
	void MoveDest(float fTimeElapsed);
	void JumpDest(float fTimeElapsed);

	void UpdatePosition();
};

