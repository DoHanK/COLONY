#pragma once
#include "stdafx.h" 
#include "ColonyGameObject.h"
#include "ColonyCamera.h"

class PlayerAnimationController;
//조작키
#define W 'W'
#define S 'S'
#define A 'A'
#define D 'D'
#define R 'R'
#define F 'F'
#define T 'T'

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
//#define 

// Addtional State
#define STATE_RELOAD				0x08	//R
#define STATE_SHOOT					0x10	//L_Mouse
#define STATE_PICK_UP				0x20	//F
#define STATE_JUMP					0x40	//SPCAE_BAR
#define STATE_SWITCH_WEAPON			0x80


enum PlayerAnimationName {
	IDLE_RIFLE,
	IDLE_GUNPLAY,
	IDLE_RELOAD,
	IDLE_PICK_UP,
	IDLE_JUMP,
	IDLE_JUMPING,
	IDLE_LANDING,
	WALK_FORWORD,
	WALK_FORWORD_LEFT,
	WALK_FORWORD_RIGHT,
	WALK_LEFT,
	WALK_RIGHT,
	WALK_BACKWORD,
	WALK_BACKWORD_LEFT,
	WALK_BACKWORD_RIGHT,
	WALK_GUNPLAY,
	WALK_RELOAD,
	WALK_PICK_UP,
	WALK_JUMP,
	WEAPON_SWITCH_BACK,
	RUNNING,
	IDLE_NORMAL
};

enum PlayerAnimationTrack {
	NOW_UPPERTRACK,
	NOW_LOWERTRACK,
	PRE_UPPERTRACK,
	PRE_LOWERTRACK
};

enum WeaponPosition {
	RIGHT_HAND,
	SPINE_BACK
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



public:
	Player(CLoadedModelInfo* ModelInfo);
	Player();
	~Player();

	//Body
	GameObject* m_RightHand;
	GameObject* m_Spine;


	GameObject m_SelectWeapon;
	char m_WeaponState = RIGHT_HAND;
	virtual void SetAnimator(PlayerAnimationController* animator);
	void SetWeapon(GameObject* Weapon);
	//입력 받은 방향키로부터 가속도 계산
	void CalVelocityFromInput(DWORD dwDirection, float fDistance);
	void AddAccel(const XMFLOAT3& xmf3Shift);
	void AddPosition(const XMFLOAT3& xmf3Shift);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};


//0: 상체 , 1: 하체 , 2:상체 전 애니메이션 , 3:하체 전 애니메이션
class PlayerAnimationController :public AnimationController {
public:
	PlayerAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
public:
	DWORD m_AnimationUpperState = IDLE_RIFLE;
	DWORD m_AnimationLowerState = IDLE_RIFLE;



	float m_nowAnimationUpperWeight = 1.0f;
	float m_PreAnimationUpperWeight = 0.0f;

	float m_nowAnimationLowerWeight = 1.0f;
	float m_PreAnimationLowerWeight = 0.0f;

	Player* m_player = NULL;
	char m_WeaponState = RIGHT_HAND;
public:
	virtual void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);
	void SetAnimationFromInput(DWORD dwDir, DWORD dwState);
	//애니메이션 상하체 동시 제어
	void ChangeAnimation(DWORD ChangeState);
	//각각 제어
	void ChangeUpperAnimation(DWORD ChangeState);
	void ChangeLowerAnimation(DWORD ChangeState);

	bool CheckLowerBody(std::string FrameName);

	bool isSameState(DWORD dwState);
	bool isSameUpperState(DWORD dwState);
	bool isSameLowerState(DWORD dwState);

	bool IsUnChangeableUpperState();
	bool IsUnChangeableLowerState();

	bool isAnimationPlayProgress(bool top,DWORD dwState, float progress);
};