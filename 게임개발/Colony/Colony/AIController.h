#pragma once
#include "AlienSpider.h"
#include "GoalThink.h"

class GoalThink;
class AlienSpider;
class AlienSpiderAnimationController;

class DogMonster;
class DogAnimationController;
class DogGoalThink;

class BossMonster;
class BossAnimationController;
class BossGoalThink;


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
	virtual bool ExecuteGoal(float fTimeElapsed);
private:
	virtual void ExecuteFollowPath(float fTimeElapsed);
	virtual void ExecuteWait(float fTimeElapsed);
	virtual void ExecuteWanderPath(float fTimeElapsed);
	virtual void MoveDest(float fTimeElapsed);
	virtual void JumpDest(float fTimeElapsed);

	void UpdatePosition();
};

class DogAIController {
public:
	DogGoalThink* m_pBrain;
	DogMonster* m_pBody;
	DogAnimationController* m_pAnimationControl;
public:
	DogAIController(DogGoalThink* pBrain, DogMonster* pBody);
	~DogAIController() {};


	//이동관련
	XMFLOAT2  m_dest;
	XMFLOAT3  m_dir;
	bool	  m_StartTravel;
	float     m_BetweenAngle;
	XMFLOAT3	  m_CrossBetween;
	//Animation


public:
	bool ExecuteGoal(float fTimeElapsed);
private:

	void ExecuteWait(float fTimeElapsed);

	void MoveDest(float fTimeElapsed);


};


class BossAIController {
public:
	BossGoalThink* m_pBrain;
	BossMonster* m_pBody;
	BossAnimationController* m_pAnimationControl;
public:
	BossAIController(BossGoalThink* pBrain, BossMonster* pBody);
	~BossAIController() {};


	//이동관련
	XMFLOAT2  m_dest;
	XMFLOAT3  m_dir;
	bool	  m_StartTravel;
	float     m_BetweenAngle;
	XMFLOAT3	  m_CrossBetween;
	//Animation


public:
	bool ExecuteGoal(float fTimeElapsed);
private:

	void ExecuteWait(float fTimeElapsed);

	void MoveDest(float fTimeElapsed);


};