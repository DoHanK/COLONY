#pragma once
#include "AlienSpider.h"
#include "Goal.h"

class WaitGoal :public Goal<AlienSpider> {

public:
	WaitGoal(AlienSpider* pOwner,float WaitingTime);
	~WaitGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};


class HittedGoal :public Goal<AlienSpider> {

public:
	HittedGoal(AlienSpider* pOwner, float WaitingTime);
	~HittedGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};


class DeadedGoal :public Goal<AlienSpider> {

public:
	DeadedGoal(AlienSpider* pOwner, float WaitingTime);
	~DeadedGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();
};

class AttackGoal :public Goal<AlienSpider> {
public:
	AttackGoal(AlienSpider* pOwner, float WaitingTime);
	~AttackGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();

};

class JumpGoal : public Goal<AlienSpider> {
public:
	JumpGoal(AlienSpider* pOwner, float WaitingTime);
	~JumpGoal();
	float m_time;
	//the usual suspects
	void Activate();
	int  Process();
	void Terminate();

};