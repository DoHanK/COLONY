#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ResourceManager.h"
#include "PathFinder.h"
#include "ColonyShader.h"
#include "GoalThink.h"
#include "AIController.h"
#define TRAILER_COUNT 5
class AlienSpiderAnimationController;

class GoalThink;
class AIController;




//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AlienSpider Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AlienSpider : public GameObject{
public:
	AlienSpider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManage,PathFinder* pPathFinder);
	virtual ~AlienSpider();

public:
	//Ai를 위한 
	GoalThink*		m_pBrain;
	AIController*	m_pSoul;
	int				m_GoalType;
	std::list<XMFLOAT2> m_Path;

	//PathManager
	PathFinder*		m_pPathFinder;
	RouteMesh* m_pRoute;
public:
	//이동
	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	void AddPostion(const XMFLOAT3& Pos);
	//idle 기다리기
	float						m_WaitingTime = 0;
	float						m_WaitCoolTime = 0;
	float						m_EndureLevel;
public:
	//GhostTrailEffect를 위한 변수들
	GameObject*			m_pSkinnedModel;
	ID3D12Resource**	m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4**		m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;
	float m_GhostNum[TRAILER_COUNT];
	float m_AniTime =0.0f;
	
	GhostTraillerShader* m_pGhostShader = NULL;
public:
	void SetGhostShader(GhostTraillerShader* pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);

	void RouteRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Update(float fTimeElapsed);

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
	void SavePrevFrameInfo(XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms, float* elapsedTime);
public:
	//애니메이션 보간 및 상태
	DWORD m_AnimationState = AlienAnimationName::Idle_1;
	float m_nowAnimationWeight = 1.0f;
	float m_PreAnimationWeight = 0.0f;

	void ChangeAnimation(DWORD ChangeState);
	bool isSameState(DWORD dwState);
	virtual void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);
};