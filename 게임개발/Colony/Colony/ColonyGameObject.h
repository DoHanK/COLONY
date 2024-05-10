#pragma once
#include "stdafx.h"
#include "ColonyShader.h"
#include "ColonyMesh.h"
#include "ResourceManager.h"
#include "ColonyGameObject.h"
class BasicShader;
class ResourceManager;
class Camera;
class BillboardShader;
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// 모델의 표면을 정의 ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//텍스쳐 리소스 타입
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

//오브젝트 텍스쳐의 SRV위치
struct SRVROOTARGUMENTINFO
{
	int								m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										Texture Class												//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class Texture {
public:
	Texture(int nTextrueResoureces = 1, UINT nResoureceType = RESOURCE_TEXTURE2D, int nSameplers = 0);
	virtual ~Texture();
private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	ID3D12Resource**				m_ppd3dTextures = NULL;
	ID3D12Resource**				m_ppd3dTextureUploadBuffers;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	SRVROOTARGUMENTINFO* m_pRootArgumentInfos = NULL;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetRootSignatureIndex(int nIndex, UINT nRootParameterIndex);
	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex, bool bIsDDSFile = true);

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	void SetTexture(int nIndex, ID3D12Resource* Texture) { m_ppd3dTextures[nIndex] = Texture; }
	UINT GetTextureType() { return(m_nTextureType); }

	void ReleaseUploadBuffers();


};


//재질 타입
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										Material Class												//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class Material
{
public:
	Material(int nTextures);
	virtual ~Material();

private:
	int								m_nReferences = 0;

public:

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	int GetRef() { return m_nReferences; }
public:
	BasicShader* m_pShader = NULL;

	XMFLOAT4						m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4						m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4						m_xmf4AmbientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	void SetShader(BasicShader* pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(Texture* pTexture, UINT nTexture = 0);

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType = 0x00;

	float							m_fGlossiness = 0.0f;
	float							m_fSmoothness = 0.0f;
	float							m_fSpecularHighlight = 0.0f;
	float							m_fMetallic = 0.0f;
	float							m_fGlossyReflection = 0.0f;

public:
	int 							m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	Texture** m_ppTextures = NULL; //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, Texture** ppTexture, GameObject* pParent, FILE* pInFile,const char* TexFileName, ResourceManager* pResourceManager);

public:
	static BasicShader* m_pStandardShader;
	static BasicShader* m_pSkinnedAnimationShader;

	static void Material::MakeShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

	void SetStandardShader() { Material::SetShader(m_pStandardShader); }
	void SetSkinnedAnimationShader() { Material::SetShader(m_pSkinnedAnimationShader); }
};





/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// 모델의 애니메이션 정의 ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG		2

#define ANIMATION_CALLBACK_EPSILON	0.015f

struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void* m_pCallbackData = NULL;
};

#define _WITH_ANIMATION_INTERPOLATION


//////////////////////////////////////////////////////////////////////////////////////////////////////
//									CAnimationCallbackHandler Class								    //
//////////////////////////////////////////////////////////////////////////////////////////////////////
class CAnimationCallbackHandler
{
public:
	CAnimationCallbackHandler() { }
	~CAnimationCallbackHandler() { }

public:
	virtual void HandleCallback(void* pCallbackData) { }
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//										AnimationSet Class											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationSet
{
public:
	AnimationSet(float fLength, int nFramesPerSecond, int nKeyFrameTransforms, int nSkinningBones, char* pstrName);
	~AnimationSet();

public:
	char							m_pstrAnimationSetName[64];

	float							m_fLength = 0.0f;
	int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

	int								m_nKeyFrames = 0;
	float* m_pfKeyFrameTimes = NULL;
	XMFLOAT4X4** m_ppxmf4x4KeyFrameTransforms = NULL;

	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY* m_pCallbackKeys = NULL;

	CAnimationCallbackHandler* m_pAnimationCallbackHandler = NULL;

public:
	void SetPosition(float fTrackPosition);

	XMFLOAT4X4 GetSRT(int nBone);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler);

	void* GetCallbackData();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										AnimationSetS Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationSets
{
private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

public:
	AnimationSets(int nAnimationSets);
	~AnimationSets();

public:
	int								m_nAnimationSets = 0;
	AnimationSet** m_ppAnimationSets = NULL;

public:
	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										LoadModelInfo Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { }
	~CLoadedModelInfo();

	GameObject* m_pModelRootObject = NULL;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMesh** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	AnimationSets** m_ppAnimationSets = NULL;

	int* m_pnAnimatedBoneFrames = NULL; //[SkinnedMeshes]
	GameObject*** m_pppAnimatedBoneFrameCaches = NULL; //[SkinnedMeshes][Bones]
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										AnimationTrack Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationTrack
{
public:
	AnimationTrack() { }
	~AnimationTrack() { }

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }
	void SetPosition(float fPosition) { m_fPosition = fPosition; }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//									AnimationController Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class AnimationController {
public:
	AnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel);
	virtual ~AnimationController();

public:
	float 							m_fTime = 0.0f;

	int 							m_nAnimationTracks = 0;
	AnimationTrack* m_pAnimationTracks = NULL;

	int 							m_nSkinnedMeshes = 0;

	AnimationSets** m_ppAnimationSets = NULL;
	SkinnedMesh** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	int* m_pnAnimatedBoneFrames = NULL;
	GameObject*** m_pppAnimatedBoneFrameCaches = NULL; //[SkinnedMeshes][Bones]

	ID3D12Resource** m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	virtual void SetTrackEnable(int nAnimationTrack, bool bEnable);
	virtual void SetTrackPosition(int nAnimationTrack, float fPosition);
	virtual void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	virtual void SetTrackWeight(int nAnimationTrack, float fWeight);

	virtual void SetCallbackKeys(int nSkinnedMesh, int nAnimationSet, int nCallbackKeys);
	virtual void SetCallbackKey(int nSkinnedMesh, int nAnimationSet, int nKeyIndex, float fTime, void* pData);
	virtual void SetAnimationCallbackHandler(int nSkinnedMesh, int nAnimationSet, CAnimationCallbackHandler* pCallbackHandler);

	virtual void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);
	virtual void DirectUpdateMatrix();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
//										GameObject Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class GameObject
{
protected:
	int								m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	GameObject();
	GameObject(int nMaterials);
	virtual ~GameObject();

public:
	char							m_pstrFrameName[64];

	BasicMesh*						m_pMesh = NULL;
	DynamicMesh*					m_pBoundingMesh = NULL;

	int								m_nMaterials = 0;
	Material**						m_ppMaterials = NULL;

	XMFLOAT4X4						m_xmf4x4ToParent;
	XMFLOAT4X4						m_xmf4x4World;

	BoundingOrientedBox  m_BoundingBox = BoundingOrientedBox();
	XMFLOAT3* FramePos = NULL;


	GameObject* m_pParent = NULL;
	GameObject* m_pChild = NULL;
	GameObject* m_pSibling = NULL;

	void SetMesh(BasicMesh* pMesh);
	void SetBoundingMesh(DynamicMesh* pMesh);
	void SetMaterial(int nMaterial, Material* pMaterial);

	void SetChild(GameObject* pChild, bool bReferenceUpdate = false);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

	
	virtual void OnPrepareAnimate() { }
	virtual void Animate(float fTimeElapsed);

	virtual void UpdateFramePos(int idex);

	virtual void UpdateMatrix() { }
	virtual void UpdateBoundingBox(ID3D12Device* pd3dDevice=NULL, ID3D12GraphicsCommandList* pd3dCommandList=NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void RenderBindAlbedo(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, Texture* Albedo);
	virtual void DepthRender(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void PerceptRangeRender(ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void BoudingBoxRender(ID3D12GraphicsCommandList* pd3dCommandList, bool isUpdateBounding = true, Camera* pCamera = NULL);
	virtual void PerceptionBindRender(ID3D12GraphicsCommandList* pd3dCommandList) {}

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, Material* pMaterial);

	virtual void ReleaseUploadBuffers();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetWorldPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	GameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	GameObject* FindFrame(char* pstrFrameName);

	Texture* FindReplicatedTexture(_TCHAR* pstrTextureName);

	UINT GetMeshType() { return((m_pMesh) ? m_pMesh->GetType() : 0x00); }

public:
	virtual void SetAnimator(AnimationController* animator);
	AnimationController* m_pSkinnedAnimationController = NULL;

	SkinnedMesh* FindSkinnedMesh(char* pstrSkinnedMeshName);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, GameObject* pParent, FILE* pInFile, BasicShader* pShader , const char* TexFileName, ResourceManager* pResouceManager);

	static void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel);
	static GameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, GameObject* pParent, FILE* pInFile, BasicShader* pShader, int* pnSkinnedMeshes , const char* TexFileName , ResourceManager* pResourceManager);

	static CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,const char* pstrFileName, BasicShader* pShader, const char* TexFileName, ResourceManager* pResourceManager);

	static void PrintFrameInfo(GameObject* pGameObject, GameObject* pParent);

};

class Player;
class SkyBoxShader;

class SkyBox : public GameObject {
public:
	SkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~SkyBox();
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera, Player* player);

	SkyBoxShader* skyBoxShader;
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										Billboard Class											   //
/////////////////////////////////////////////////////////////////////////////////////////////////////

class Billboard :public GameObject {
public:
	Billboard(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, Texture* texture,
		BillboardShader* pShader, GameObject* ownerObject=NULL);
	~Billboard();
	BillBoardMesh* m_BillMesh;
	Material* BillboardMaterial;
	XMFLOAT3 m_OffsetPos = XMFLOAT3(0,0,0);
	// Set
	float m_rows = 8;
	float m_cols = 8;

	float m_row = 0;
	float m_col = 0;

	float SettedTimer = 0.5f;
	float Timer = 0;
	bool active = true;
	bool doAnimate = false;
	bool doOnce = false;
	BoundingSphere* m_CrashObject = NULL;
	XMFLOAT3 TickAddPosition =XMFLOAT3(0.0f, 0.0f, 0.0f);
	GameObject* m_ownerObject;
	void SetRowNCol(float row, float col) { m_rows = row; m_cols = col; }
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera);
	void Update(XMFLOAT3 xmf3Target, XMFLOAT3 xmf3Up);
	void SetAddPosition(XMFLOAT3 xmf3AddPosition) {TickAddPosition = xmf3AddPosition;}
	void SetOffsetPos(const XMFLOAT3& Pos) { m_OffsetPos = Pos; };
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//										ParticleObject Class									   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class ParticleShader;
class ParticleObject :public GameObject {
public:
	ParticleObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, Texture* pTexture, ParticleShader* pShader,
		XMFLOAT3 position, UINT type, XMFLOAT3 direction, XMFLOAT3 acc, float speed, float lifeTime, float age, float startTime, UINT nMaxParticles);
	virtual ~ParticleObject();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, Camera* pCamera = NULL);
	virtual void PostRender(int nPipelineState);
	ID3D12Device* m_device;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//										ItemObject Class										   //
/////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemObject :public GameObject {
public:
	ItemObject(){}
	virtual ~ItemObject(){}
public:
	bool m_bActive = false;
};