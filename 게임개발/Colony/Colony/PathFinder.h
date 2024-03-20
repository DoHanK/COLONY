#pragma once
#include "stdafx.h"
#include "NevMeshBaker.h"
#include "PriorityQueue.h"

class EdgeInfo {
public:
	EdgeInfo(int From, int To, float cost):m_iFrom(From), m_ito(To),m_dCost(cost) {};
	int m_iFrom;
	int m_ito;
	float m_dCost;
};

class PathFinder{
public:
	PathFinder() {};
	~PathFinder();
public:
	std::vector<XMFLOAT2> m_Node;
	std::vector<std::list<EdgeInfo>> m_Edge;
	int m_widthCount;
	int m_HeightCount;
	std::vector<Cell> m_Cell;


public:
	void BuildGraphFromCell(std::vector<Cell>& pCell, int WidthCount, int HeightCount);
	void addAdjacentNodes(Cell* pCell,int col ,int row);
	bool ValidAdjacnet(int x, int y);
	float CalDistance(XMFLOAT2 a, XMFLOAT2 b);
	
	int BringIndexCell(const XMFLOAT3& pos);
	
	std::list<XMFLOAT2> QueryPath(XMFLOAT3 ObjectPos);
	std::list<XMFLOAT2> QueryClosePath(XMFLOAT3 ObjectPos);

	
	bool ValidNode(int ind) { 
		if (ind == -1) return false;
		return m_Cell[ind].m_Pass; }


};


class AStarAlgoritm {
public:
	AStarAlgoritm(PathFinder* pPthFinder, int Source, int Target) :m_Source(Source), m_Target(Target),
																		m_GCosts(pPthFinder->m_Node.size(), 0.0),
																		m_FCosts(pPthFinder->m_Node.size(), 0.0),
																		m_ShortestPathTree(pPthFinder->m_Node.size()),
																		m_SearchFrontier(pPthFinder->m_Node.size()),
																		m_pPathFinder(pPthFinder)
	{
		Search();
	};
	PathFinder*					m_pPathFinder;


	int  m_Target;
	int  m_Source;

	std::vector<double>             m_GCosts;
	std::vector<double>             m_FCosts;
	std::vector<const EdgeInfo*>       m_ShortestPathTree;
	std::vector<const EdgeInfo*>       m_SearchFrontier;


	void Search();

	std::list<int> GetPathToTarget();

};
