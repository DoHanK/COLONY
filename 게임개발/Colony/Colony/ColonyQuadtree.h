#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"
#include "ColonyMesh.h"
#include "AlienSpider.h"
#include "CollisionManager.h"

enum QTDirection { LEFT_UP, RIGHT_UP, LEFT_BOTTOM, RIGHT_BOTTOM };
class QuadTree
{
public:

	QuadTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int depth, XMFLOAT3 center, XMFLOAT3 Extend);
	~QuadTree();
protected:
	int								m_nReferences = 0;
public:
	void AddRef();
	void Release();
public:
	int							m_depth;
	BoundingBox					m_BoundingBox;
	std::vector<GameObject*>	m_StaticObject;
	std::list<GameObject*>	m_DynamicObject;
	std::list<DogMonster*>	m_SubObject;
	std::vector<int>			m_Route; // ��Ʈ ã��

	//	
	std::vector<Collision*> m_StaticBoundings;


	QuadTree* m_LeftUp = NULL;
	QuadTree* m_RightUp = NULL;
	QuadTree* m_LeftBottom = NULL;
	QuadTree* m_RightBottom = NULL;

	int			m_SameDepthidx = 0;

	BoundingBoxMesh* m_BoundingMesh = NULL;

	Camera* m_pCamera;
	Player* m_pPlayer;
	BCapsule* m_pBoundPlayer;


	vector<Billboard*>*					m_DeadEneyEffect;
	vector <XMFLOAT3>					m_itemRespons;
public:



	bool BuildTreeByDepth(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int limitdepth);

	void InsertStaticObject(std::vector<GameObject*> object);

	void InsertDynamicObject();

	void SettingStaticBounding(CollisionManager& collisionManager);

	void BoundingRendering(ID3D12GraphicsCommandList* pd3dCommandList, int DepthLevel);

	void BringDepthTrees(std::vector< QuadTree*>& out, int ndepth);

	void CalSameDepthidx();

	void AnimateObjects(float elapsedTime, vector<GameObject*>& Enemys, vector<DogMonster*>& DogEnemy);

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void CollisionEnemyToStaticObject();
	void CollisionEnemyToPlayer();
};

