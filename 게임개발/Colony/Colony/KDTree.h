#pragma once
#include "stdafx.h"
#include "ColonyGameObject.h"

class Node {
public:
	float m_right = 0;
	float m_left = 0;
	float m_top = 0;
	float m_bottom = 0;
	vector<GameObject*> m_pStaticObject;
	vector<GameObject*> m_pDynamicObject;
	GameObject* m_pPivotObject;
	int m_NodeDepth = 0;


	Node* m_pNodeRight = NULL;
	Node* m_pNodeLeft = NULL;

	Node* m_pNodeDown = NULL;
	Node* m_pNodeUp = NULL;

};

class KDTree{
public:
	KDTree(int depth,float top ,float bottom , float right , float left);
public:
	Node *m_pNode;
	int m_Depth;

public:
	void CreateNode(Node* parrent , int depth);
	void UpdateNode(Node* parrent, int depth);
};

