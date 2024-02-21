#include "PathFinder.h"

void PathFinder::BuildGraphFromCell(Cell* pCell, int WidthCount, int HeightCount)
{
	m_widthCount = WidthCount;
	m_HeightCount = HeightCount;
	m_Cell = pCell;
	m_Edge.resize(m_widthCount * m_HeightCount);
	for (int y = 0; y < HeightCount; y++) {
		
		for (int x = 0; x < WidthCount; x++) {

			m_Node.push_back({ pCell[x + WidthCount * y].m_BoundingBox.Center.x, pCell[x + WidthCount * y].m_BoundingBox.Center.z });

			if(pCell[x + WidthCount * y].m_Pass) addAdjacentNodes(pCell, x, y);

		}
	}


}


void PathFinder::TargetNSourceRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//색깔선정
	XMFLOAT3 xmfloat3(0.0f, 0.0f, 1.0f);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 3, &xmfloat3, 36);
	XMFLOAT4X4 xmf4x4World = Matrix4x4::Identity();
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
	int count = 0;
	for (auto mesh : m_Mesh) {
		if (count == m_count) break;
		mesh->Render(pd3dCommandList);
		count++;
	}
}

void PathFinder::addAdjacentNodes( Cell* pCell, int col , int row)
{
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			int nodeX = col + j;
			int nodeY = row + i;

			if ((i == 0) && (j == 0)) continue;

			if (ValidAdjacnet(nodeX, nodeY)) {
				
				int NowNodeNum = col + row * m_widthCount;
				Cell* NowNode = &pCell[NowNodeNum];
				XMFLOAT2 NowNodePos(NowNode->m_BoundingBox.Center.x, NowNode->m_BoundingBox.Center.z);

				int AdjacentNodeNum = nodeX + nodeY * m_widthCount;
				Cell* AdjacentNode = &pCell[AdjacentNodeNum];
				XMFLOAT2 AdjacentNodePos(AdjacentNode->m_BoundingBox.Center.x, AdjacentNode->m_BoundingBox.Center.z);

				if (AdjacentNode->m_Pass) 	
				{
		
						EdgeInfo E(NowNodeNum, AdjacentNodeNum, CalDistance(NowNodePos, AdjacentNodePos));
						m_Edge[col + row * m_widthCount].push_back(E);
				
				
				}
			}

		}
	}
}

float PathFinder::CalDistance(XMFLOAT2 a, XMFLOAT2 b)
{
	return sqrt((a.x -b.x) * (a.x - b.x)+ (a.y - b.y) * (a.y - b.y));
}

bool PathFinder::ValidAdjacnet(int x, int y)
{
	return !((x < 0) || (x >= m_widthCount) || (y < 0) || (y >= m_HeightCount));
}

void PathFinder::GetAstarPath(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int s , int t)
{

	srand(time(0));
	while (!ValidNode(s)) {
		s = rand() % m_Node.size();
	}
	m_s = 19169;
	while (!ValidNode(t)) {
		t = rand() % m_Node.size();
	}


	m_t = t;
	AStarAlgoritm Astar(this, m_s, m_t);

	for (int i = 0; i  < 5000; i++) {

		BoundingBoxMesh* pMesh = new BoundingBoxMesh(pd3dDevice, pd3dCommandList);
		m_Mesh.push_back(pMesh);
	}

	

}


void PathFinder::GetPathRendom()
{

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, m_Node.size());
	m_s = dis(gen);
	m_t = dis(gen);
	while (!ValidNode(m_s)) {
		m_s = dis(gen);
	}
	while (!ValidNode(m_t)) {
		m_t = dis(gen);
	}
	OutputDebugStringA("값들(M: ,");
	OutputDebugStringA(to_string(m_t).c_str());

	OutputDebugStringA("T: ");
	OutputDebugStringA(to_string(m_s).c_str());
	OutputDebugStringA(")\n");
	AStarAlgoritm Astar(this, m_s, m_t);
	std::list<int> path = Astar.GetPathToTarget();
	m_count = 0;
	for (auto l : path) {

		
		m_Mesh[m_count++]->UpdateVertexPosition(&m_Cell[l].m_BoundingBox);
		if (5000 < m_count) {
			break;
		}
	}

}


void AStarAlgoritm::Search()
{
	IndexPriorityQueue pq(m_FCosts, m_pPathFinder->m_Node.size());

	pq.insert(m_Source);


	while (!pq.empty())
	{

		int NextClosestNode = pq.Pop();

		//노드 따라 이동
		m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

		if (NextClosestNode == m_Target) return;

		//방문 노드 확장
		for (const auto &Edge : m_pPathFinder->m_Edge[NextClosestNode]) {

			//현재 노드로부터 목적지까지 예상 거리
			double Hcost = m_pPathFinder->CalDistance(m_pPathFinder->m_Node[m_Target], m_pPathFinder->m_Node[NextClosestNode]);
			//실제 출발지점으로부터 현재 노드까지 이동거리
			double GCost = m_GCosts[NextClosestNode] + Edge.m_dCost;

			if (m_SearchFrontier[Edge.m_ito] == NULL) {
				m_FCosts[Edge.m_ito] = GCost + Hcost;
				m_GCosts[Edge.m_ito] = GCost;

				pq.insert(Edge.m_ito);

				m_SearchFrontier[Edge.m_ito] = &Edge;
			}

			else if ((GCost < m_GCosts[Edge.m_ito]) && (m_ShortestPathTree[Edge.m_ito] == NULL))
			{
				m_FCosts[Edge.m_ito] = GCost + Hcost;
				m_GCosts[Edge.m_ito] = GCost;

				pq.ChangePriority(Edge.m_ito);

				m_SearchFrontier[Edge.m_ito] = &Edge;
			}


		}

	}

}



std::list<int> AStarAlgoritm::GetPathToTarget() {

	std::list<int> path;

	//just return an empty path if no target or no path found
	if (m_Target < 0)  return path;

	int nd = m_Target;

	path.push_front(nd);

	while ((nd != m_Source) && (m_ShortestPathTree[nd] != 0))
	{
		nd = m_ShortestPathTree[nd]->m_iFrom;

		path.push_front(nd);
	}

	return path;
}
