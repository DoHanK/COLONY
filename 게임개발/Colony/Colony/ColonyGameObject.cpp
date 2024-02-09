#include "stdafx.h"
#include "ColonyMesh.h"
#include "ColonyGameObject.h"
#include "ColonyShader.h"
#include "ResourceManager.h"
#include "ColonyPlayer.h"
class PlayerAnimationController;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//												Desc											    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
//							Texture Class ,Matiral Class -> Matrial									//
// 												   													//
//		CAnimationCallbackHandler , AnimationSet , AnimationSets ,AnimationController  -> Animation	//  
//  																								//
//						CLoadedModelInfo , GameObject -> Model & Object in Game						//				  		 
//////////////////////////////////////////////////////////////////////////////////////////////////////

BasicShader* Material::m_pStandardShader;
BasicShader* Material::m_pSkinnedAnimationShader;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										Texture Class												//
//////////////////////////////////////////////////////////////////////////////////////////////////////
Texture::Texture(int nTextrueResoureces, UINT nResoureceType, int nSameplers)
{
	m_nTextureType = nResoureceType;
	m_nTextures = nTextrueResoureces;

	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
	}

	m_nSamplers = nSameplers;
	if (m_nSamplers > 0) m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

Texture::~Texture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void Texture::SetRootSignatureIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
}

void Texture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
}

void Texture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void Texture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nTextureType == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void Texture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex)
{
	
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void Texture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

void Texture::ReleaseShaderVariables()
{

}

void Texture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex, bool bIsDDSFile)
{
	if (bIsDDSFile)
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	else
		m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromWICFile(pd3dDevice, pd3dCommandList, pszFileName, &(m_ppd3dTextureUploadBuffers[nIndex]), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										Material Class												//
//////////////////////////////////////////////////////////////////////////////////////////////////////
Material::Material(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new Texture * [m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

Material::~Material()
{
	if (m_pShader) m_pShader->Release();

	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}
}

void Material::SetShader(BasicShader* pShader)
{
	 if (m_pShader) m_pShader->Release(); 

	 if (pShader) {
		 m_pShader = pShader;
		 m_pShader->AddRef();
	 }
}

void Material::SetTexture(Texture* pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}

void Material::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

void Material::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0);
	}
}
//#define _WITH_DISPLAY_TEXTURE_NAME
void Material::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, Texture** ppTexture, GameObject* pParent, FILE* pInFile,const char* TexFileName, ResourceManager* pResourceManager)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = ::ReadStringFromFile(pInFile, pstrTextureName);

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		SetMaterialType(nType);

		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, TexFileName);
		//strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + strlen(TexFileName), 64 - strlen(TexFileName), (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + strlen(TexFileName) + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - strlen(TexFileName) - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, pwstrTextureName, 64, pstrFilePath, _TRUNCATE);

		

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', pwstrTextureName);
		OutputDebugString(pstrDebug);
#endif


		* ppTexture = pResourceManager->BringTexture(pstrFilePath, nRootParameter, false);
		if (*ppTexture) (*ppTexture)->AddRef();

		//if (!bDuplicated)
		//{

	
		//	*ppTexture = pResourceManager->BringTexture(pstrFilePath, nRootParameter, false);
		//	if (*ppTexture) (*ppTexture)->AddRef();

		//	//ResourceManager::CreateShaderResourceViews(pd3dDevice, *ppTexture, nRootParameter, false);
		//}
		//else
		//{
		//	if (pParent)
		//	{
		//		while (pParent)
		//		{
		//			if (!pParent->m_pParent) break;
		//			pParent = pParent->m_pParent;
		//		}
		//		GameObject* pRootGameObject = pParent;
		//		*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
		//		if (*ppTexture) (*ppTexture)->AddRef();
		//	}
		//}
	}
}

void Material::MakeShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_pStandardShader = new StandardShader();
	m_pStandardShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pStandardShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pSkinnedAnimationShader = new SkinnedAnimationStandardShader();
	m_pSkinnedAnimationShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pSkinnedAnimationShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AnimationSet Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
AnimationSet::AnimationSet(float fLength, int nFramesPerSecond, int nKeyFrames, int nSkinningBones, char* pstrName)
{
	m_fLength = fLength;
	m_nFramesPerSecond = nFramesPerSecond;
	m_nKeyFrames = nKeyFrames;
	strcpy_s(m_pstrAnimationSetName, 64, pstrName);

	m_pfKeyFrameTimes = new float[nKeyFrames];
	m_ppxmf4x4KeyFrameTransforms = new XMFLOAT4X4 * [nKeyFrames];
	for (int i = 0; i < nKeyFrames; i++) m_ppxmf4x4KeyFrameTransforms[i] = new XMFLOAT4X4[nSkinningBones];

}

AnimationSet::~AnimationSet()
{
	if (m_pfKeyFrameTimes) delete[] m_pfKeyFrameTimes;
	for (int j = 0; j < m_nKeyFrames; j++) if (m_ppxmf4x4KeyFrameTransforms[j]) delete[] m_ppxmf4x4KeyFrameTransforms[j];
	if (m_ppxmf4x4KeyFrameTransforms) delete[] m_ppxmf4x4KeyFrameTransforms;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

void* AnimationSet::GetCallbackData()
{
	for (int i = 0; i < m_nCallbackKeys; i++)
	{
		//if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON)) return(m_pCallbackKeys[i].m_pCallbackData);
	}
	return(NULL);
}

void AnimationSet::SetPosition(float fTrackPosition)
{
	m_fPosition = fTrackPosition;
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		m_fPosition = fmod(fTrackPosition, m_pfKeyFrameTimes[m_nKeyFrames - 1]); 
		// m_fPosition = fTrackPosition - int(fTrackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			m_fPosition = fmod(fTrackPosition, m_fLength); //if (m_fPosition < 0) m_fPosition += m_fLength;
		//			m_fPosition = fTrackPosition - int(fTrackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE_ONCE:
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}

	//if (m_pAnimationCallbackHandler)
	//{
	//	void* pCallbackData = GetCallbackData();
	//	if (pCallbackData) m_pAnimationCallbackHandler->HandleCallback(pCallbackData);
	//}
}

XMFLOAT4X4 AnimationSet::GetSRT(int nBone)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameTranslationTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i + 1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nBone]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i + 1][nBone]), t);
			break;
		}
	}
	for (int i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameScaleTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i + 1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nBone]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i + 1][nBone]), t);
			break;
		}
	}
	for (int i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameRotationTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i + 1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nBone]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i + 1][nBone]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
#else   
	if (m_nKeyFrames ==  1) {

		return(m_ppxmf4x4KeyFrameTransforms[0][nBone]);
	}
	for (int i = 0; i < (m_nKeyFrames - 1); i++)
	{
		if ((m_pfKeyFrameTimes[i] <= m_fPosition) && (m_fPosition <= m_pfKeyFrameTimes[i + 1]))
		{
			float t = (m_fPosition - m_pfKeyFrameTimes[i]) / (m_pfKeyFrameTimes[i + 1] - m_pfKeyFrameTimes[i]);
			xmf4x4Transform = Matrix4x4::Interpolate(m_ppxmf4x4KeyFrameTransforms[i][nBone], m_ppxmf4x4KeyFrameTransforms[i + 1][nBone], t);

			break;
		}
	}
#endif
	return(xmf4x4Transform);
}

void AnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void AnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void AnimationSet::SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										AnimationSetS Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
AnimationSets::AnimationSets(int nAnimationSets)
{
	m_nAnimationSets = nAnimationSets;
	m_ppAnimationSets = new AnimationSet * [nAnimationSets];
}

AnimationSets::~AnimationSets()
{
	for (int i = 0; i < m_nAnimationSets; i++) if (m_ppAnimationSets[i]) delete m_ppAnimationSets[i];
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
}

void AnimationSets::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_ppAnimationSets[nAnimationSet]->m_nCallbackKeys = nCallbackKeys;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void AnimationSets::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_ppAnimationSets[nAnimationSet]->m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void AnimationSets::SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler)
{
	m_ppAnimationSets[nAnimationSet]->SetAnimationCallbackHandler(pCallbackHandler);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										LoadModelInfo Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
CLoadedModelInfo::~CLoadedModelInfo()
{
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;
	if (m_pnAnimatedBoneFrames) delete[] m_pnAnimatedBoneFrames;
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		if (m_pppAnimatedBoneFrameCaches[i]) delete[] m_pppAnimatedBoneFrameCaches[i];
	}
	if (m_pppAnimatedBoneFrameCaches) delete[] m_pppAnimatedBoneFrameCaches;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										GameObject Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
GameObject::GameObject()
{
	
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
	m_ppMaterials = NULL;
}

GameObject::GameObject(int nMaterials) : GameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new Material * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

GameObject::~GameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_pBoundingMesh) m_pBoundingMesh->Release();

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}

	if (m_ppMaterials) delete[] m_ppMaterials;

	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void GameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void GameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

void GameObject::SetChild(GameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void GameObject::SetMesh(BasicMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::SetBoundingMesh(DynamicMesh* pMesh)
{
	
	if (m_pBoundingMesh) m_pBoundingMesh->Release();
	m_pBoundingMesh = pMesh;
	if (m_pBoundingMesh) m_pBoundingMesh->AddRef();
}

void GameObject::SetMaterial(int nMaterial, Material* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

void GameObject::SetAnimator(AnimationController* animator){
	if (animator) {
		if (m_pSkinnedAnimationController) {
			delete m_pSkinnedAnimationController;
		}

		m_pSkinnedAnimationController = animator;
	}
}

SkinnedMesh* GameObject::FindSkinnedMesh(char* pstrSkinnedMeshName)
{
	SkinnedMesh* pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		pSkinnedMesh = (SkinnedMesh*)m_pMesh;
		if (strlen(pSkinnedMesh->m_pstrMeshName) == strlen(pstrSkinnedMeshName))
			if (!strncmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName, strlen(pstrSkinnedMeshName))) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return(NULL);
}

GameObject* GameObject::FindFrame(char* pstrFrameName)
{
	GameObject* pFrameObject = NULL;
	if(strlen(m_pstrFrameName)== strlen(pstrFrameName))
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void GameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void GameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
		if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void GameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

void GameObject::Animate(float fTimeElapsed)
{
	UpdateMatrix();

	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->DirectUpdateMatrix();

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);



}

void GameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{
	//프레임으로 이뤄진 것이기에 필요가 없음
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);


	if (m_pMesh)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
						m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i);

			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);
}

void GameObject::BoudingBoxRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera)
{

	if (m_pBoundingMesh) {
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);
		m_pBoundingMesh->Render(pd3dCommandList);
	}

	if (m_pSibling) m_pSibling->BoudingBoxRender(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->BoudingBoxRender(pd3dCommandList, pCamera);
}

void GameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void GameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);

}

void GameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, Material* pMaterial)
{
}

void GameObject::ReleaseShaderVariables()
{
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}

void GameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}



void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

XMFLOAT3 GameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void GameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void GameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

//#define _WITH_DEBUG_FRAME_HIERARCHY

Texture* GameObject::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	Texture* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
}


void GameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pParent, FILE* pInFile, BasicShader* pShader , const char* TexFileName,ResourceManager* pResouceManager)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	m_nMaterials = ReadIntegerFromFile(pInFile);

	m_ppMaterials = new Material * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	Material* pMaterial = NULL;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(pInFile);

			pMaterial = new Material(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
			else {
				pMaterial->SetShader(pShader);
			}
			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 3, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile , TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 4, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 5, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 6, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 7, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 8, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 9, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, pInFile, TexFileName, pResouceManager);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

GameObject* GameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, GameObject* pParent, FILE* pInFile, BasicShader* pShader, int* pnSkinnedMeshes, const char* TexFileName,ResourceManager* pResourceManager)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	GameObject* pGameObject = new GameObject();

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(pInFile);
			nTextures = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			StandardMesh* pMesh = new StandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			SkinnedMesh* pSkinnedMesh = new SkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

			pGameObject->SetMesh(pSkinnedMesh);

		}													
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader, TexFileName, pResourceManager);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					GameObject* pChild = GameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes ,TexFileName, pResourceManager);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	if (pGameObject->m_pMesh) {
		pGameObject->m_BoundingBox.Center = pGameObject->m_pMesh->GetAABBCenter();
		pGameObject->m_BoundingBox.Extents = pGameObject->m_pMesh->GetAABBExtend();
		pGameObject->m_pBoundingMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
		pGameObject->m_pBoundingMesh->AddRef();
		((BoundingBoxMesh*)pGameObject->m_pBoundingMesh)->UpdateVertexPosition(&pGameObject->m_BoundingBox);
	}

	
	return(pGameObject);
}

void GameObject::PrintFrameInfo(GameObject* pGameObject, GameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) GameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) GameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

void GameObject::LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			pLoadedModel->m_nSkinnedMeshes = ::ReadIntegerFromFile(pInFile);

			pLoadedModel->m_pnAnimatedBoneFrames = new int[pLoadedModel->m_nSkinnedMeshes];
			pLoadedModel->m_ppAnimationSets = new AnimationSets * [pLoadedModel->m_nSkinnedMeshes];
			pLoadedModel->m_ppSkinnedMeshes = new SkinnedMesh * [pLoadedModel->m_nSkinnedMeshes];
			pLoadedModel->m_pppAnimatedBoneFrameCaches = new GameObject * *[pLoadedModel->m_nSkinnedMeshes];

			for (int i = 0; i < pLoadedModel->m_nSkinnedMeshes; i++)
			{
				pLoadedModel->m_ppAnimationSets[i] = new AnimationSets(nAnimationSets);



				::ReadStringFromFile(pInFile, pstrToken); //Skinned Mesh Name
				pLoadedModel->m_ppSkinnedMeshes[i] = pLoadedModel->m_pModelRootObject->FindSkinnedMesh(pstrToken);
				//스키닝 모델만 해당 모델들 연결해줌
				if (pLoadedModel->m_ppSkinnedMeshes[i])
					pLoadedModel->m_ppSkinnedMeshes[i]->PrepareSkinning(pLoadedModel->m_pModelRootObject);

				pLoadedModel->m_pppAnimatedBoneFrameCaches[i] = new GameObject*;
				pLoadedModel->m_pppAnimatedBoneFrameCaches[i][0] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

				pLoadedModel->m_pnAnimatedBoneFrames[i] = 1; //자식이 없음
	
				#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pLoadedModel->m_pppAnimatedBoneFrameCaches[i][0]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
				#endif
		
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{

			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fLength = ::ReadFloatFromFile(pInFile);
			int nFramesPerSecond = ::ReadIntegerFromFile(pInFile);
			int nKeyFrames = ::ReadIntegerFromFile(pInFile);

			for (int i = 0; i < pLoadedModel->m_nSkinnedMeshes; i++)//[프레임] [애니메이션Set]   
			{
				pLoadedModel->m_ppAnimationSets[i]->m_ppAnimationSets[nAnimationSet] = new AnimationSet(fLength, nFramesPerSecond, nKeyFrames, pLoadedModel->m_pnAnimatedBoneFrames[i], pstrToken);
			}

			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(pInFile, pstrToken);

				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					int nKey = ::ReadIntegerFromFile(pInFile); //i
					float fKeyTime = ::ReadFloatFromFile(pInFile);
					for (int j = 0; j < pLoadedModel->m_nSkinnedMeshes; j++) //프레임 메쉬 수만큼 반복문이 돌아감.
					{
						//int nSkin = ::ReadIntegerFromFile(pInFile); //j
						AnimationSet* pAnimationSet = pLoadedModel->m_ppAnimationSets[j]->m_ppAnimationSets[nAnimationSet];//각 프레임마다 해당 값 넣어주는중.
						pAnimationSet->m_pfKeyFrameTimes[i] = fKeyTime;
						nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(XMFLOAT4X4), pLoadedModel->m_pnAnimatedBoneFrames[j], pInFile);

					
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

CLoadedModelInfo* GameObject::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,const char* pstrFileName, BasicShader* pShader, const char* TexFileName,ResourceManager* pResourceManager)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo* pLoadedModel = new CLoadedModelInfo();

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				pLoadedModel->m_pModelRootObject = GameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes , TexFileName, pResourceManager);
				::ReadStringFromFile(pInFile, pstrToken); //"</Hierarchy>"
			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				GameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pLoadedModel);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									AnimationController Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
AnimationController::AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new AnimationTrack[nAnimationTracks];

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;

	m_ppAnimationSets = new AnimationSets * [m_nSkinnedMeshes];
	m_pnAnimatedBoneFrames = new int[m_nSkinnedMeshes];
	m_ppSkinnedMeshes = new SkinnedMesh * [m_nSkinnedMeshes];
	m_pppAnimatedBoneFrameCaches = new GameObject * *[m_nSkinnedMeshes];

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

		m_ppAnimationSets[i] = pModel->m_ppAnimationSets[i];
		m_ppAnimationSets[i]->AddRef();

		m_pnAnimatedBoneFrames[i] = pModel->m_pnAnimatedBoneFrames[i];

		m_pppAnimatedBoneFrameCaches[i] = new GameObject * [m_pnAnimatedBoneFrames[i]];
		for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
		{
			m_pppAnimatedBoneFrameCaches[i][j] = pModel->m_pppAnimatedBoneFrameCaches[i][j];
		}
	}

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		if (m_ppSkinnedMeshes[i]) {
				m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
				m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);		
		}

	}


}

AnimationController::~AnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;
	if (m_pnAnimatedBoneFrames) delete[] m_pnAnimatedBoneFrames;

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		if (m_ppAnimationSets[i]) m_ppAnimationSets[i]->Release();
		if (m_pppAnimatedBoneFrameCaches[i]) delete[] m_pppAnimatedBoneFrameCaches[i];
		if (m_ppSkinnedMeshes[i]) {
			m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
			m_ppd3dcbSkinningBoneTransforms[i]->Release();
		}
	}

	if (m_pppAnimatedBoneFrameCaches) delete[] m_pppAnimatedBoneFrameCaches;
	if (m_ppAnimationSets) delete[] m_ppAnimationSets;

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;

	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;
}

void AnimationController::SetCallbackKeys(int nSkinnedMesh, int nAnimationSet, int nCallbackKeys)
{
	if (m_ppAnimationSets && m_ppAnimationSets[nSkinnedMesh]) m_ppAnimationSets[nSkinnedMesh]->SetCallbackKeys(nAnimationSet, nCallbackKeys);
}

void AnimationController::SetCallbackKey(int nSkinnedMesh, int nAnimationSet, int nKeyIndex, float fKeyTime, void* pData)
{
	if (m_ppAnimationSets && m_ppAnimationSets[nSkinnedMesh]) m_ppAnimationSets[nSkinnedMesh]->SetCallbackKey(nAnimationSet, nKeyIndex, fKeyTime, pData);
}

void AnimationController::SetAnimationCallbackHandler(int nSkinnedMesh, int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler)
{
	if (m_ppAnimationSets && m_ppAnimationSets[nSkinnedMesh]) m_ppAnimationSets[nSkinnedMesh]->SetAnimationCallbackHandler(nAnimationSet, pCallbackHandler);
}

void AnimationController::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pAnimationTracks) {
		m_pAnimationTracks[nAnimationTrack].m_nAnimationSet = nAnimationSet;
	}
}

void AnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
}

void AnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void AnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void AnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void AnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		if (m_ppSkinnedMeshes[i]) {
			m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
			m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
		}
	}
}

void AnimationController::AdvanceTime(float fTimeElapsed, GameObject* pRootGameObject)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; i++) 
			m_pAnimationTracks[i].m_fPosition += (fTimeElapsed * m_pAnimationTracks[i].m_fSpeed);

		for (int i = 0; i < m_nSkinnedMeshes; i++)
		{
			for (int j = 0; j < m_pnAnimatedBoneFrames[i]; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Zero();
				for (int k = 0; k < m_nAnimationTracks; k++)
				{
					if (m_pAnimationTracks[k].m_bEnable)
					{
						AnimationSet* pAnimationSet = m_ppAnimationSets[i]->m_ppAnimationSets[m_pAnimationTracks[k].m_nAnimationSet];
						pAnimationSet->SetPosition(m_pAnimationTracks[k].m_fPosition);
						XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j);
						xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[k].m_fWeight));
					}
				}
				m_pppAnimatedBoneFrameCaches[i][j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
		}

		pRootGameObject->UpdateTransform(NULL);
	}
}

void AnimationController::DirectUpdateMatrix()
{

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		if (m_ppSkinnedMeshes[i]) {
			
			for (int j = 0; j < m_ppSkinnedMeshes[i]->m_nSkinningBones; j++) {
			XMStoreFloat4x4(&m_ppcbxmf4x4MappedSkinningBoneTransforms[i][j], XMMatrixTranspose(XMLoadFloat4x4(&m_ppSkinnedMeshes[i]->m_ppSkinningBoneFrameCaches[j]->m_xmf4x4World)));

			}
		}
	}

}
