#include "AlienSpider.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
//											AlineSipder												//
// 																									//
//////////////////////////////////////////////////////////////////////////////////////////////////////
		  		 

std::mutex modellock;
std::mutex animationlock;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AlienSpider Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
AlienSpider::AlienSpider(ID3D12Device* pd3dDevice , ID3D12GraphicsCommandList* pd3dCommandList, ResourceManager* pResourceManager, PathFinder* pPathFinder)
{

	CLoadedModelInfo* pSpider = pResourceManager->BringModelInfo("Model/AlienSpider.bin", "Model/");
	AlienSpiderAnimationController* pAnimationSpider = new AlienSpiderAnimationController(pd3dDevice, pd3dCommandList, 2, pSpider);
	pAnimationSpider->SetTrackAnimationSet(0, 0);
	SetPosition(XMFLOAT3( 0, 0, 0));
	SetChild(pSpider->m_pModelRootObject, true);
	SetAnimator(pAnimationSpider);
	m_pSkinnedModel = pSpider->m_pModelRootObject->FindFrame("polySurface10");


	//m_pBrain
	m_pBrain = new GoalThink(this);
	//Soul
	m_pSoul = new AIController(m_pBrain, this);
	m_pSoul->m_pAnimationControl = ((AlienSpiderAnimationController*)m_pSkinnedAnimationController);
	m_pPathFinder = pPathFinder;
	m_pRoute = new RouteMesh(pd3dDevice, pd3dCommandList);
	m_pRoute->AddRef();
	//Alien 특성
	m_EndureLevel = floatEndureDis(gen);

	//인식
	m_pPerception = new Perception(this);

	FramePos = new XMFLOAT3[AlienboneIndex::End];

	m_pHead = pSpider->m_pModelRootObject->FindFrame("DEF-HEAD");

	//m_pBoundingMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
	//m_pBoundingMesh->AddRef();
	m_BoundingBox = pSpider->m_pModelRootObject->FindFrame("polySurface10")->m_BoundingBox;

	//Ghost Effect
	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [TRAILER_COUNT];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [TRAILER_COUNT];
	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < TRAILER_COUNT; ++i) {
		m_GhostNum[i] = 0;
		
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}



}

AlienSpider::~AlienSpider()
{
	//Brain
	if (m_pBrain) delete m_pBrain;
	if (m_pSoul) delete m_pSoul;
	//perception
	if (m_pPerception) delete m_pPerception;
	if(m_pRoute) m_pRoute->Release();


	//Ghost Effect 
	for (int i = 0; i < TRAILER_COUNT; ++i) {
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;
	
	if (FramePos)  delete[] FramePos;

}

void AlienSpider::Animate(float fTimeElapsed)
{
	m_AniTime += fTimeElapsed;
	static int count = 0;
	static int stdcount = 1;
	//폐기되기전 기록 저장
	if (m_AniTime > m_AniCoolTime) {
		m_GhostNum[0] = 0.f;
		if (count > stdcount) {
			((AlienSpiderAnimationController*)m_pSkinnedAnimationController)->SavePrevFrameInfo(m_ppcbxmf4x4MappedSkinningBoneTransforms, m_GhostNum);

			count = 0;
			stdcount = rand()%2+1;
		}
		for (int i = 0; i < TRAILER_COUNT; i++) {
			m_GhostNum[i] += fTimeElapsed;
		}
		m_AniTime = 0;
		m_AniCoolTime = (rand() / RAND_MAX)/100.f+0.05f ;
		count++;
	}


	UpdateMatrix();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);
	

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);

	if(FramePos){

		//몸통
		UpdateFramePos(AlienboneIndex::DEF_HIPS);
		UpdateFramePos(AlienboneIndex::DEF_SPINE_1);
		UpdateFramePos(AlienboneIndex::DEF_CHEST);
		UpdateFramePos(AlienboneIndex::DEF_NECK_1);
		UpdateFramePos(AlienboneIndex::DEF_HEAD);
		UpdateFramePos(AlienboneIndex::DEF_TAIL);
		UpdateFramePos(AlienboneIndex::DEF_TAIL_001);

		//다리
		UpdateFramePos(AlienboneIndex::DEF_LEG_BACK_02_L);
		UpdateFramePos(AlienboneIndex::DEF_LEG_BACK_02_R);
		UpdateFramePos(AlienboneIndex::DEF_LEG_FRONT_02_L);
		UpdateFramePos(AlienboneIndex::DEF_LEG_FRONT_02_R);
		UpdateFramePos(AlienboneIndex::DEF_LEG_MIDDLE_02_L);
		UpdateFramePos(AlienboneIndex::DEF_LEG_MIDDLE_02_R);
		UpdateFramePos(AlienboneIndex::DEF_FORARM_L);
		UpdateFramePos(AlienboneIndex::DEF_FORARM_R);
	}
}

void AlienSpider::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);

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
	RenderBindAlbedo(pd3dCommandList, NULL, m_pSpiderTex);

	GhostTrailerRender(pd3dCommandList, NULL);

}

void AlienSpider::GhostTrailerRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//GhostTrailer Effect
	if (m_pSkinnedModel->m_pMesh) {

		if (m_pSkinnedModel->m_nMaterials > 0)
		{
			for (int i = 0; i < m_pSkinnedModel->m_nMaterials; i++)
			{
				if (m_pSkinnedModel->m_ppMaterials[i]) {

					if (m_pGhostShader) m_pGhostShader->OnPrepareRender(pd3dCommandList, 0);
					//m_pSkinnedModel->m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
					if(m_pGhostMaskTex) m_pGhostMaskTex->UpdateShaderVariable(pd3dCommandList, 0);
				}
				for (int j = TRAILER_COUNT - 1; j > -1; --j) {

					pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1,  &m_GhostNum[j], 33);

					D3D12_GPU_VIRTUAL_ADDRESS d3dcbBoneTransformsGpuVirtualAddress = m_ppd3dcbSkinningBoneTransforms[j]->GetGPUVirtualAddress();
					pd3dCommandList->SetGraphicsRootConstantBufferView(12, d3dcbBoneTransformsGpuVirtualAddress); //Skinned Bone Transforms

					m_pSkinnedModel->m_pMesh->Render(pd3dCommandList, 0);

				}

			}
		}
	}
}

void AlienSpider::PerceptionBindRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pPerceptionRangeMesh)m_pPerceptionRangeMesh->PerceptionBindRender(pd3dCommandList);
}

void AlienSpider::PerceptRangeRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if(m_pPerceptionRangeMesh)m_pPerceptionRangeMesh->PerceptionRangeRender(pd3dCommandList,0);
}

void AlienSpider::RouteRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pRoute) {
		m_pRoute->Render(pd3dCommandList);
	}
}

void AlienSpider::BoudingBoxRender(ID3D12GraphicsCommandList* pd3dCommandList, bool isUpdateBounding, Camera* pCamera)
{
	if (m_pBoundingMesh) {
		BoundingOrientedBox MoveBox;
		m_BoundingBox.Transform(MoveBox, DirectX::XMLoadFloat4x4(&m_xmf4x4World));

		((BoundingBoxMesh*)m_pBoundingMesh)->UpdateVertexPosition(&MoveBox);

		m_pBoundingMesh->Render(pd3dCommandList);
	}

}

void AlienSpider::Update(float fTimeElapsed)
{
	//판단	
	m_pBrain->Process();
	//실행
	m_pSoul->ExecuteGoal(fTimeElapsed);
	//위치 변경
	UpdatePosition(fTimeElapsed);

}

void AlienSpider::UpdatePosition(float fTimeElapsed)
{
	XMFLOAT3 xmf3Gravity = XMFLOAT3(0, -9.8f, 0);


	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(xmf3Gravity, fTimeElapsed, false));
	// 
	//마찰계수
	float  fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = 5.f;

	if (fLength > fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);

	}



	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	if ((m_GoalType == Deaded_Goal ||
		m_GoalType == Hitted_Goal ||
		m_GoalType == Attack_Goal) && !m_pBrain->m_bJump) {
		m_xmf3Velocity.x = 0;
		m_xmf3Velocity.z = 0;

	}
	
	XMFLOAT3 PrePos = XMFLOAT3(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43);
	m_xmfPre3Position = PrePos;
	AddPostion(xmf3Velocity);
	m_xmfPre3Velocity = xmf3Velocity;

	

}

void AlienSpider::AnimateWithMultithread(float fTimeElapsed, int idx)
{
	m_AniTime += fTimeElapsed;
	static int count = 0;
	static int stdcount = 1;
	//폐기되기전 기록 저장
	if (m_AniTime > m_AniCoolTime) {
		m_GhostNum[0] = 0.f;
		if (count > stdcount) {
			((AlienSpiderAnimationController*)m_pSkinnedAnimationController)->SavePrevFrameInfo(m_ppcbxmf4x4MappedSkinningBoneTransforms, m_GhostNum);

			count = 0;
			stdcount = rand() % 2 + 1;
		}
		for (int i = 0; i < TRAILER_COUNT; i++) {
			m_GhostNum[i] += fTimeElapsed;
		}
		m_AniTime = 0;
		m_AniCoolTime = (rand() / RAND_MAX) / 100.f + 0.05f;
		count++;
	}


	UpdateTransformWithMultithread(NULL, idx,  0);

	if (m_pSkinnedAnimationController)((AlienSpiderAnimationController*)m_pSkinnedAnimationController)->AdvanceTimeWithMultithread(fTimeElapsed, this,idx);


	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);

	if (FramePos) {

		//몸통
		UpdateFramePos(AlienboneIndex::DEF_HIPS, idx);
		UpdateFramePos(AlienboneIndex::DEF_SPINE_1, idx);
		UpdateFramePos(AlienboneIndex::DEF_CHEST, idx);
		UpdateFramePos(AlienboneIndex::DEF_NECK_1, idx);
		UpdateFramePos(AlienboneIndex::DEF_HEAD, idx);
		UpdateFramePos(AlienboneIndex::DEF_TAIL, idx);
		UpdateFramePos(AlienboneIndex::DEF_TAIL_001, idx);

		//다리
		UpdateFramePos(AlienboneIndex::DEF_LEG_BACK_02_L, idx);
		UpdateFramePos(AlienboneIndex::DEF_LEG_BACK_02_R, idx);
		UpdateFramePos(AlienboneIndex::DEF_LEG_FRONT_02_L, idx);
		UpdateFramePos(AlienboneIndex::DEF_LEG_FRONT_02_R, idx);
		UpdateFramePos(AlienboneIndex::DEF_LEG_MIDDLE_02_L, idx);
		UpdateFramePos(AlienboneIndex::DEF_LEG_MIDDLE_02_R, idx);
		UpdateFramePos(AlienboneIndex::DEF_FORARM_L, idx);
		UpdateFramePos(AlienboneIndex::DEF_FORARM_R, idx);
	}

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
		elapsedTime[i] = elapsedTime[i - 1];
	}

	memcpy(ppcbxmf4x4MappedSkinningBoneTransforms[0], m_ppcbxmf4x4MappedSkinningBoneTransforms[1], ncbElementBytes);

	
	
}

void AlienSpiderAnimationController::ChangeAnimation(DWORD ChangeState)
{
	m_nowAnimationWeight = 0.0f;

	SetTrackAnimationSet(PRE_TRACK, m_AnimationState);

	m_AnimationState = ChangeState;
	SetTrackAnimationSet(NOW_TRACK, ChangeState);


	m_pAnimationTracks[PRE_TRACK].m_fPosition = m_pAnimationTracks[NOW_TRACK].m_fPosition;
	m_pAnimationTracks[NOW_TRACK].m_fPosition = 0.0f;
}

bool AlienSpiderAnimationController::isSameState(DWORD dwState)
{
	if (m_AnimationState == dwState) return true;
	return false;
}

void AlienSpiderAnimationController::AdvanceTime(float fElapsedTime, GameObject* pRootGameObject)
{
	m_fTime += fElapsedTime;

	if (m_nowAnimationWeight < 1.0f) {
		m_nowAnimationWeight += fElapsedTime*2;
		m_PreAnimationWeight = 1.0f - m_nowAnimationWeight;

	}
	else {
		m_nowAnimationWeight = 1.0f;
		m_PreAnimationWeight = 0.0f;
	}
	SetTrackWeight(NOW_TRACK, m_nowAnimationWeight);
	SetTrackWeight(PRE_TRACK, m_PreAnimationWeight);


	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++) {
			//상체
			//if (m_nowAnimationWeight == 1 && (i == NOW_TRACK || i == PRE_TRACK)) {
				m_pAnimationTracks[i].m_fPosition += (fElapsedTime * m_pAnimationTracks[i].m_fSpeed);
			//}
				//OutputDebugStringA(to_string(m_pAnimationTracks[i].m_fSpeed).c_str());
				//OutputDebugStringA("   ");
	
		}
		std::vector < XMFLOAT4X4> tempmatrix;
		//OutputDebugStringA("  \n ");
		for (int i = 0; i < m_nSkinnedMeshes; i++)
		{
			for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();
			

				for (int k = 0; k < m_nAnimationTracks; k++)
				{
					animationlock.lock();
					AnimationSet* pAnimationSet = m_ppAnimationSets[i]->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
					pAnimationSet->SetPosition(m_pAnimationTracks[k].m_fPosition);
					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
					animationlock.unlock();
				
					xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));

				}
				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "DEF-HIPS") {
					xmf4x4Transform._41 = 1.72889e-06;
					xmf4x4Transform._42 = -0.0001055449;
					xmf4x4Transform._43 = 0.007409086;
				}

				tempmatrix.push_back(xmf4x4Transform);

			}

		}

		modellock.lock();
		auto p = tempmatrix.begin();
		for (int i = 0; i < m_nSkinnedMeshes; i++)
		{
			for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
			{
				m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = *p;
				p++;

			}

		}

		pRootGameObject->UpdateTransform(NULL);
		DirectUpdateMatrix();
		modellock.unlock();

	}


}

bool AlienSpiderAnimationController::isAnimationPlayProgress(DWORD dwState, float progress)
{
	if(m_pAnimationTracks[NOW_TRACK].m_nAnimationSet == dwState)
	if (m_pAnimationTracks[NOW_TRACK].m_fPosition / m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength >= progress) {
		return true;
	}
	

	return false;
}

void AlienSpiderAnimationController::SetAnimationPlayPos(DWORD dwState, float progress)
{
	m_pAnimationTracks[NOW_TRACK].m_fPosition = progress * m_ppAnimationSets[0]->m_ppAnimationSets[dwState]->m_fLength;
	

}

void AlienSpiderAnimationController::AdvanceTimeWithMultithread(float fElapsedTime, GameObject* pRootGameObject, int idx)
{

	m_fTime += fElapsedTime;

	if (m_nowAnimationWeight < 1.0f) {
		m_nowAnimationWeight += fElapsedTime * 2;
		m_PreAnimationWeight = 1.0f - m_nowAnimationWeight;

	}
	else {
		m_nowAnimationWeight = 1.0f;
		m_PreAnimationWeight = 0.0f;
	}
	SetTrackWeight(NOW_TRACK, m_nowAnimationWeight);
	SetTrackWeight(PRE_TRACK, m_PreAnimationWeight);


	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++) {
			//상체
			//if (m_nowAnimationWeight == 1 && (i == NOW_TRACK || i == PRE_TRACK)) {
			m_pAnimationTracks[i].m_fPosition += (fElapsedTime * m_pAnimationTracks[i].m_fSpeed);
			//}
				//OutputDebugStringA(to_string(m_pAnimationTracks[i].m_fSpeed).c_str());
				//OutputDebugStringA("   ");

		}
		std::vector < XMFLOAT4X4> tempmatrix;
		//OutputDebugStringA("  \n ");
		for (int i = 0; i < m_nSkinnedMeshes; i++)
		{
			for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();


				for (int k = 0; k < m_nAnimationTracks; k++)
				{
					AnimationSet* pAnimationSet = m_ppAnimationSets[i]->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
					animationlock.lock();
					pAnimationSet->SetPosition(m_pAnimationTracks[k].m_fPosition);
					XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
					animationlock.unlock();

					xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));

				}
				if (string(m_pppAnimatedBoneFrameCaches[i][j]->m_pstrFrameName) == "DEF-HIPS") {
					xmf4x4Transform._41 = 1.72889e-06;
					xmf4x4Transform._42 = -0.0001055449;
					xmf4x4Transform._43 = 0.007409086;
				}
				m_pppAnimatedBoneFrameCaches[i][j]->m_xmfsub4x4ToParent[idx] = xmf4x4Transform;

			}

		}


		pRootGameObject->UpdateTransformWithMultithread(NULL,idx , 0);
		DirectUpdateMatrixWithMultithread(idx);

	}

}
