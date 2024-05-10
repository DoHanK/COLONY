#pragma once
#include "stdafx.h" 
#include "ColonyGameObject.h"

class ThirdPersonCamera;
class PlayerAnimationController;


//Player Acceleration Speed Control
#define PlayerRunAcel  2.0f;
#define NoGrapAcel 5.0f;

//조작키
#define W 'W'
#define S 'S'
#define A 'A'
#define D 'D'
#define R 'R'
#define F 'F'
#define T 'T'
#define Q 'Q'
#define P 'P'

#define L_MOUSE		 0x01
#define R_MOUSE		 0x02
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

// Weapon State
#define HAVE_RIFLE					0
#define HAVE_SHOTGUN				1
#define HAVE_MACHINEGUN				2	



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										Player Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class Player :public GameObject
{
protected:
	
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;


	
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;


	ThirdPersonCamera* m_pCamera = NULL;
public:
	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmfPre3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmfPre3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

public:
	Player(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,ResourceManager* pResourceManager);
	~Player();


	//Body
	GameObject* m_RightHand;
	GameObject* m_Spine;

	GameObject m_SelectWeapon;
	char m_WeaponState = RIGHT_HAND;
	
	bool isJump = false;
	bool isDance = false;

	float m_BaseReloadTime = 0.1f;
	float m_ReloadTime = 0;


	// Weapon
	GameObject*	m_UMP5Object = NULL;
	GameObject* m_shotgunObject = NULL;
	GameObject* m_machinegunObject = NULL;

	// Bullet num
	UINT m_gunType = HAVE_RIFLE;
	int m_rifleBullet = 40;
	int m_shotgunBullet = 20;
	int m_machinegunBullet = 150;


	virtual void SetAnimator(PlayerAnimationController* animator);
	void SetWeapon(GameObject* Weapon);


	//입력 받은 방향키로부터 가속도 계산
	void SetPosition(const XMFLOAT3& Position);
	void CalVelocityFromInput(DWORD dwDirection, float Acceleration, float fElapsedTime);
	void AddAccel(const XMFLOAT3& xmf3Shift);
	void AddPosition(const XMFLOAT3& xmf3Shift);
	void UpdatePosition(float fTimeElapsed);
	XMFLOAT3 GetVelocity() { return m_xmf3Velocity; }
	//입력받은 회전 처리
	void Rotate(float x, float y, float z);
	virtual void UpdateMatrix();
public:
	void SetMaxXZVelocity(const float& veclocity) {m_fMaxVelocityXZ = veclocity;}
public:
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	void SetLookVector(const XMFLOAT3& LookVector) { m_xmf3Look = LookVector; }
	void SetUpVector(const XMFLOAT3& UpVector) { m_xmf3Up = UpVector; }
	void SetRightVector(const XMFLOAT3& RightVector) { m_xmf3Right = RightVector; }
	void SetCamera(ThirdPersonCamera* pCamera);
	void RollbackPosition() { m_xmf3Position = m_xmfPre3Position; }
	ThirdPersonCamera* GetCamera() { return m_pCamera; }

	virtual void ReleaseUploadBuffers();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//								PlayerAnimationController Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
//0: 상체 , 1: 하체 , 2:상체 전 애니메이션 , 3:하체 전 애니메이션
class PlayerAnimationController :public AnimationController {
public:
	PlayerAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~PlayerAnimationController(){}
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