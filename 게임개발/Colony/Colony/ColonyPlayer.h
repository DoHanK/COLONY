#pragma once
#include "stdafx.h" 
#include "ColonyGameObject.h"
#include "ColonyCamera.h"

//조작키
#define W 'W'
#define S 'S'
#define A 'A'
#define D 'D'
#define R 'R'
#define F 'F'

#define L_MOUSE		 0x01
#define L_SHIFT	     0x10
#define L_CONTROL	 0x11
#define SPACE_BAR    0x20


// 플레이어 키입력에 따른 이동 방향
#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_JUMP_UP					0x10

// Player Animation State
// basic State

#define STATE_IDLE					0x01	//NONE
#define STATE_WALK					0x02	//W A S D 
#define STATE_RUN					0x04	//L_SHIFT

// Addtional State
#define STATE_RELOAD				0x08	//R
#define STATE_SHOOT					0x10	//L_Mouse
#define STATE_PICK_UP				0x20	//F
#define STATE_JUMP					0x40	//SPCAE_BAR

enum PlayerStateName {
	IDLE_RIFLE,
	IDLE_GUNPLAY,
	IDLE_RELOAD,
	IDLE_PICK_UP,
	//IDLE_JUMP,
	//IDLE_LANDING,
	WALK_FORWARD,
	WALK_BACKWORD,
	WALK_LEFT,
	WALK_RIGHTWORD,
	WALK_GUNPLAY,
	WALK_RELOAD,
	WALK_PICK_UP,
	WALK_JUMP
};


class Player :public GameObject
{
protected:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;


	Camera* m_pCamera = NULL;

	DWORD  m_AnimationState = IDLE_RIFLE;

public:
	Player();
	~Player();

	//입력 받은 방향키로부터 가속도 계산
	void CalVelocityFromInput(DWORD dwDirection, float fDistance);
	void AddAccel(const XMFLOAT3& xmf3Shift);
	void AddPosition(const XMFLOAT3& xmf3Shift);
	void SetAnimationFromInput(DWORD dwir , DWORD dwState);
};

