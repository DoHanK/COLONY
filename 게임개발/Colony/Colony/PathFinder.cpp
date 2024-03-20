#include "PathFinder.h"

PathFinder::~PathFinder()
{

}

void PathFinder::BuildGraphFromCell(std::vector<Cell>& pCell, int WidthCount, int HeightCount)
{
	m_widthCount = WidthCount;
	m_HeightCount = HeightCount;
	m_Cell = pCell;
	m_Edge.resize(m_widthCount * m_HeightCount);
	for (int y = 0; y < HeightCount; y++) {
		
		for (int x = 0; x < WidthCount; x++) {

			m_Node.push_back({ pCell[x + WidthCount * y].m_BoundingBox.Center.x, pCell[x + WidthCount * y].m_BoundingBox.Center.z });

			if(pCell[x + WidthCount * y].m_Pass) addAdjacentNodes(&pCell[0], x, y);

		}
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

int PathFinder::BringIndexCell(const XMFLOAT3& pos)
{
	int x = floor((pos.x + H_MAPSIZE_X ) / CELL_SIZE) ;
	int y = floor((pos.z + H_MAPSIZE_Y )/ CELL_SIZE) ;

	return (x  + m_widthCount * y);
}

std::list<XMFLOAT2> PathFinder::QueryPath(XMFLOAT3 ObjectPos)
{

	std::uniform_int_distribution<> dis(0, m_Node.size());
	
	int DestIndex = dis(gen);

	int StartIndex = BringIndexCell(ObjectPos);
	
	float Min = FLT_MAX;
	//���� ������ ��ȿ���� ������
	if (!ValidNode(StartIndex)) {

		XMFLOAT2  StartCoord = XMFLOAT2(ObjectPos.x, ObjectPos.z);

		for (int i = 0; i < m_Node.size(); ++i) {
			if (m_Cell[i].m_Pass) {

				XMFLOAT2  DestCoord = XMFLOAT2(m_Cell[i].m_BoundingBox.Center.x, m_Cell[i].m_BoundingBox.Center.z);

				float fmin = XM2CalDis(StartCoord, DestCoord);

				if (fmin < Min) {
					Min = fmin;
					StartIndex = i;
				}

			}
		}

	}

	std::list<int> path;
	while (true) {
		while (!ValidNode(DestIndex)) {
			DestIndex = dis(gen);
		}

		AStarAlgoritm Astar(this, StartIndex, DestIndex);
		 path = Astar.GetPathToTarget();
		if (path.size()>1) {
			break;
		}
		DestIndex = -1;
	}

	std::list<XMFLOAT2> XMPath;
	for (const auto& nodeIndex : path) {
		XMFLOAT2 Coord = XMFLOAT2(m_Cell[nodeIndex].m_BoundingBox.Center.x, m_Cell[nodeIndex].m_BoundingBox.Center.z);

		
		XMPath.push_back(Coord);
	}

	return XMPath;
}

std::list<XMFLOAT2> PathFinder::QueryClosePath(XMFLOAT3 ObjectPos)
{
	float Min = FLT_MAX;
	//���� ������ ��ȿ���� ������

	int StartIndex = BringIndexCell(ObjectPos);
	int nodeX = 0;
	int nodeY = 0;

	if (!ValidNode(StartIndex)) {

		XMFLOAT2  StartCoord = XMFLOAT2(ObjectPos.x, ObjectPos.z);

		for (int i = 0; i < m_Node.size(); ++i) {
			if (m_Cell[i].m_Pass) {

				XMFLOAT2  DestCoord = XMFLOAT2(m_Cell[i].m_BoundingBox.Center.x, m_Cell[i].m_BoundingBox.Center.z);

				float fmin = XM2CalDis(StartCoord, DestCoord);

				if (fmin < Min) {
					Min = fmin;
					StartIndex = i;
					
				}

			}
		}

	}
	nodeX = StartIndex % m_widthCount;
	nodeY = StartIndex / m_widthCount;
	


	//���� 0~ 1.6 * range����ŭ�� ��ȿ�� ã��
	std::list<XMFLOAT2> XMPath;
	for (int range = 1; range < 5; ++range) {

		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				if ((i == 0) && (j == 0)) continue;

				int X = nodeX + j*range;
				int Y = nodeY + i*range;

				if (ValidAdjacnet(X, Y)) { //��ȿ�ϸ�
					int index = X + Y * m_widthCount;
					if (m_Cell[index].m_Pass) {
						XMPath.push_back(XMFLOAT2(m_Cell[index].m_BoundingBox.Center.x, m_Cell[index].m_BoundingBox.Center.z));
					}

				}
	

			}
		}

	}
	// std::list���� std::vector�� ��ҵ��� ����
	std::vector<DirectX::XMFLOAT2> tempVec(XMPath.begin(), XMPath.end());

	// std::vector�� �������� ����
	std::shuffle(tempVec.begin(), tempVec.end(), gen);

	// std::vector�� std::list�� �ٽ� �ű�
	XMPath.assign(tempVec.begin(), tempVec.end());



	return XMPath;
	
}


void AStarAlgoritm::Search()
{
	IndexPriorityQueue pq(m_FCosts, m_pPathFinder->m_Node.size());

	pq.insert(m_Source);


	while (!pq.empty())
	{

		int NextClosestNode = pq.Pop();

		//��� ���� �̵�
		m_ShortestPathTree[NextClosestNode] = m_SearchFrontier[NextClosestNode];

		if (NextClosestNode == m_Target) return;

		//�湮 ��� Ȯ��
		for (const auto &Edge : m_pPathFinder->m_Edge[NextClosestNode]) {

			//���� ���κ��� ���������� ���� �Ÿ�
			double Hcost = m_pPathFinder->CalDistance(m_pPathFinder->m_Node[m_Target], m_pPathFinder->m_Node[NextClosestNode]);
			//���� ����������κ��� ���� ������ �̵��Ÿ�
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
