#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ResourceManager.h"
#include "ColonyShader.h"
#include "GoalThink.h"
#define TRAILER_COUNT 5
class AlienSpiderAnimationController;

class GoalThink;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AlienSpider Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AlienSpider : public GameObject{
public:
	AlienSpider(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManage);
	virtual ~AlienSpider();
public:
	GoalThink* m_pBrain;
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

};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//								AlienSpiderAnimationController Class								//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AlienSpiderAnimationController :public AnimationController {
public:
	AlienSpiderAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~AlienSpiderAnimationController() {};

public:
	void SavePrevFrameInfo(XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms, float* elapsedTime);

};