#include "AlienSpider.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
//											AlineSipder												//
// 																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////
		  		 




//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AlienSpider Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
AlienSpider::AlienSpider(ID3D12Device* pd3dDevice , ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManager, PathFinder* pPathFinder)
{

	CLoadedModelInfo* pSpider = pResourceManager->BringModelInfo("Model/AlienSpider.bin", "Model/");
	AlienSpiderAnimationController* pAnimationSpider = new AlienSpiderAnimationController(pd3dDevice, pd3dCommandList, 1, pSpider);
	pAnimationSpider->SetTrackAnimationSet(0, 0);
	SetPosition(XMFLOAT3( 0, 0, 0));
	SetChild(pSpider->m_pModelRootObject, true);
	SetAnimator(pAnimationSpider);
	m_pSkinnedModel = pSpider->m_pModelRootObject->FindFrame("polySurface10");


	//m_pBrain
	m_pBrain = new GoalThink(this);
	//Soul
	m_pSoul = new AIController(m_pBrain, this);
	
	m_pPathFinder = pPathFinder;
	m_pRoute = new RouteMesh(pd3dDevice, pd3dCommandList);
	m_pRoute->AddRef();
	//Alien 특성
	m_EndureLevel = floatEndureDis(gen);


	//Ghost Effect
	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [TRAILER_COUNT];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [TRAILER_COUNT];
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < TRAILER_COUNT; ++i) {
		m_GhostNum[i] = i;
		
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}

	
}

AlienSpider::~AlienSpider()
{
	//Brain
	if (m_pBrain) delete m_pBrain;
	if (m_pSoul) delete m_pSoul;
	if(m_pRoute) m_pRoute->Release();


	//Ghost Effect 
	for (int i = 0; i < TRAILER_COUNT; ++i) {
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;
	

}

void AlienSpider::Animate(float fTimeElapsed)
{
	m_AniTime += fTimeElapsed;

	//폐기되기전 기록 저장
	if (m_AniTime > 0.08f) {
		((AlienSpiderAnimationController*)m_pSkinnedAnimationController)->SavePrevFrameInfo(m_ppcbxmf4x4MappedSkinningBoneTransforms , m_GhostNum);
		m_AniTime = 0;
	}
	GameObject::Animate(fTimeElapsed);
}

void AlienSpider::AddPostion(const XMFLOAT3& Pos)
{
	m_xmf4x4ToParent._41 += Pos.x;
	m_xmf4x4ToParent._42 += Pos.y;
	m_xmf4x4ToParent._43 += Pos.z;

}

void AlienSpider::SetGhostShader(GhostTraillerShader* pShader)
{
	m_pGhostShader = pShader;
}

void AlienSpider::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	GameObject::Render(pd3dCommandList);

	//GhostTrailer Effect
	if (m_pSkinnedModel->m_pMesh) {

		if (m_pSkinnedModel->m_nMaterials > 0)
		{
			for (int i = 0; i < m_pSkinnedModel->m_nMaterials; i++)
			{
				if (m_pSkinnedModel->m_ppMaterials[i]){
			   		
					if (m_pGhostShader) m_pGhostShader->OnPrepareRender(pd3dCommandList, 0);
					m_pSkinnedModel->m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}
				for (int j = TRAILER_COUNT-1; j > -1; --j) {

					//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1,  &m_GhostNum[j], 33);
					//((SkinnedMesh*)m_pSkinnedModel->m_pMesh)->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[j];
					//	m_pSkinnedModel->m_pMesh->Render(pd3dCommandList, 0);

				}

			}
		}
	}

}

void AlienSpider::RouteRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pRoute) {
		m_pRoute->Render(pd3dCommandList);
	}
}

void AlienSpider::Update(float fTimeElapsed)
{
	m_pBrain->Process();

	m_pSoul->ExecuteGoal(fTimeElapsed);


}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//								AlienSpiderAnimationController Class								//
//////////////////////////////////////////////////////////////////////////////////////////////////////
AlienSpiderAnimationController::AlienSpiderAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel):
AnimationController(pd3dDevice, pd3dCommandList,  nAnimationTracks, pModel)
{
}

void AlienSpiderAnimationController::SavePrevFrameInfo(XMFLOAT4X4** ppcbxmf4x4MappedSkinningBoneTransforms ,float* elapsedTime)
{

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = TRAILER_COUNT-1; 0 < i; --i) {
		memcpy(ppcbxmf4x4MappedSkinningBoneTransforms[i], ppcbxmf4x4MappedSkinningBoneTransforms[i-1], ncbElementBytes);
		//elapsedTime[i] = elapsedTime[i - 1];
	}

	memcpy(ppcbxmf4x4MappedSkinningBoneTransforms[0], m_ppcbxmf4x4MappedSkinningBoneTransforms[1], ncbElementBytes);

	
	
}
