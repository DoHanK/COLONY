#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ResourceManager.h"
#include "PathFinder.h"
#include "ColonyShader.h"
#include "GoalThink.h"
#include "AIController.h"
#include "PerceptionMesh.h"
#include "Perception.h"
#define TRAILER_COUNT 5

class AlienSpiderAnimationController;

class GoalThink;
class DogGoalThink;
class AIController;
class Perception;
class DogAIController;



//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AlienSpider Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AlienSpider : public GameObject{
public:
	AlienSpider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManage,PathFinder* pPathFinder);
	AlienSpider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManage,PathFinder* pPathFinder,float scale);
	virtual ~AlienSpider();

public:
	//Ai를 위한 
	GoalThink*		m_pBrain;
	AIController*	m_pSoul;
	int				m_GoalType;
	std::list<XMFLOAT2> m_Path;

	//PathManager
	PathFinder*					m_pPathFinder;
	RouteMesh*					m_pRoute;

	//시야 범위
	PerceptionRangeMesh*		m_pPerceptionRangeMesh = NULL;
	Perception*					m_pPerception = NULL;
	Player*						m_pPlayer = NULL;
	float						m_fPAangle = 0.0f;
	Texture*					m_pSpiderTex = NULL;
	Texture*					m_pGhostMaskTex = NULL;
	

public:
	float           			m_fMaxVelocityXZ = 3.0f;
	void						AddPostion(const XMFLOAT3& Pos);
	//idle 기다리기
	float						m_WaitingTime = 0;
	float						m_WaitCoolTime = 0;
	float						m_EndureLevel;
	float						m_StuckTime = 0.0f;

	//인식
	GameObject*		m_pHead = NULL;
public:
	bool						m_OnceDead = false;

public:
	//GhostTrailEffect를 위한 변수들
	GameObject*			m_pSkinnedModel;
	ID3D12Resource**	m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4**		m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;
	float m_GhostNum[TRAILER_COUNT];
	float m_AniTime =0.0f;
	float m_AniCoolTime = 0.0f;
	GhostTraillerShader* m_pGhostShader = NULL;
public:
	void SetPerceptionRangeMesh(PerceptionRangeMesh* pPerceptMesh) { m_pPerceptionRangeMesh = pPerceptMesh; };
	void SetGhostShader(GhostTraillerShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void GhostTrailerRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);

	void PerceptionBindRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void PerceptRangeRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void RouteRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BoudingBoxRender(ID3D12GraphicsCommandList* pd3dCommandList, bool isUpdateBounding = true, Camera* pCamera = NULL);
	void Update(float fTimeElapsed);
	void UpdatePosition(float fTimeElapsed);

	//Multithread
	virtual void AnimateWithMultithread(float fTimeElapsed, int idx);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//								AlienSpiderAnimationController Class								//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AlienSpiderAnimationController :public AnimationController {
public:
	AlienSpiderAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~AlienSpiderAnimationController() {};

public:
	//GhostEffect
	virtual void SavePrevFrameInfo(XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms, float* elapsedTime);
public:
	//애니메이션 보간 및 상태
	DWORD m_AnimationState = 0;
	float m_nowAnimationWeight = 1.0f;
	float m_PreAnimationWeight = 0.0f;

	virtual void ChangeAnimation(DWORD ChangeState);
	virtual bool isSameState(DWORD dwState);
	virtual void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);
	virtual bool isAnimationPlayProgress(DWORD dwState, float progress);
	virtual void SetAnimationPlayPos(DWORD dwState, float progress);

	virtual void AdvanceTimeWithMultithread(float fElapsedTime, GameObject* pRootGameObject, int idx);
};




class DogMonster : public GameObject {
public:
	DogMonster(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManager, float scale);
	virtual ~DogMonster() {};
public:
	DogGoalThink* m_pBrain;
	DogAIController* m_pSoul;
	void Update(float fTimeElapsed);
	void UpdatePosition(float fTimeElapsed);
	void AnimateWithMultithread(float fTimeElapsed, int idx);

};


class DogAnimationController :public AlienSpiderAnimationController {
public:
	DogAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~DogAnimationController() {};


public:
	//애니메이션 보간 및 상태
	DWORD m_AnimationState = Spit_idle;
	float m_nowAnimationWeight = 1.0f;
	float m_PreAnimationWeight = 0.0f;


	virtual void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);
	virtual void AdvanceTimeWithMultithread(float fElapsedTime, GameObject* pRootGameObject, int idx);
};