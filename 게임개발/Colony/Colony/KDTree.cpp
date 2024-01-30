#include "KDTree.h"


KDTree::KDTree(int depth, float top, float bottom, float right, float left)
{
	m_Depth = depth;
	m_pNode = new Node;
	m_pNode->m_NodeDepth = depth;
	m_pNode->m_top = top;
	m_pNode->m_bottom = bottom;
	m_pNode->m_right = right;
	m_pNode->m_left = left;

}

void KDTree::CreateNode(Node* parrent , int depth)
{
	if (depth < m_Depth) {
		Node* pNode1 = new Node;
		pNode1->m_NodeDepth = depth;

		Node* pNode2 = new Node;
		pNode2->m_NodeDepth = depth;

		// 0 is KDTree 
		//right and left
		//x Partion
		if (depth % 2 == 1) {
			parrent->m_pNodeRight = pNode1;
			CreateNode(pNode1, depth + 1);
			parrent->m_pNodeLeft = pNode2;
			CreateNode(pNode2, depth + 1);
		}
		//up and down
		//y Partion
		else if (depth % 2 == 0 && depth != 0) {

			parrent->m_pNodeUp = pNode1;
			CreateNode(pNode1, depth + 1);
			parrent->m_pNodeDown = pNode2;
			CreateNode(pNode2, depth + 1);
		}


	}

}

void KDTree::UpdateNode(Node* parent, int depth)
{

	if ((parent->m_pDynamicObject.size() != 0) || (parent->m_pStaticObject.size() != 0)) {
		//파티션을 나누기 위함

		// X 좌표
		if (depth % 2 == 1) {
			std::vector<GameObject*> all;
			//사이즈만큼 늘려주기
			int ObjectSize = parent->m_pDynamicObject.size() + parent->m_pStaticObject.size();
			int PibotObject = 0;
			//2개이상일때 
			if (ObjectSize > 2) {
				all.resize(ObjectSize);
				//동적 오브젝트 합치기
				all.insert(std::next(parent->m_pDynamicObject.begin(), parent->m_pDynamicObject.size()), parent->m_pDynamicObject.begin(), parent->m_pDynamicObject.end());
				//정적 오브젝트 합치기
				all.insert(std::next(parent->m_pStaticObject.begin(), parent->m_pStaticObject.size()), parent->m_pStaticObject.begin(), parent->m_pStaticObject.end());
				//x로 정렬
				sort(all.begin(), all.end(), [](GameObject* a, GameObject* b) {return a->m_xmf4x4World._41 < b->m_xmf4x4World._41; });

				if (ObjectSize % 2 == 0) {
					//짝수
					PibotObject = ObjectSize + 1;

				}
				else {
					PibotObject = ObjectSize;
				}
				parent->m_pNodeRight->m_pStaticObject.clear();
				parent->m_pNodeLeft->m_pStaticObject.clear();
				//동적 오브젝트 분류 
				for (const auto& mem : parent->m_pDynamicObject) {
					//오른쪽
					if (mem->m_xmf4x4World._41 > all[PibotObject]->m_xmf4x4World._41) {
						parent->m_pNodeRight->m_pDynamicObject.push_back(mem);
					}
					//왼쪽
					else if(mem->m_xmf4x4World._41 < all[PibotObject]->m_xmf4x4World._41){
						parent->m_pNodeLeft->m_pDynamicObject.push_back(mem);
					}

				}

				//정적 오브젝트 분류 
				for (const auto& mem : parent->m_pStaticObject) {
					//오른쪽
					if (mem->m_xmf4x4World._41 > all[PibotObject]->m_xmf4x4World._41) {
						parent->m_pNodeRight->m_pStaticObject.push_back(mem);
					}
					//왼쪽
					else if (mem->m_xmf4x4World._41 < all[PibotObject]->m_xmf4x4World._41) {
						parent->m_pNodeLeft->m_pStaticObject.push_back(mem);
					}

				}
				UpdateNode(parent->m_pNodeRight, depth + 1);
				UpdateNode(parent->m_pNodeLeft, depth + 1);

			}
			else {

			}

		}
		// Z 좌표
		else if (depth % 2 == 0 && depth != 0) {
			std::vector<GameObject*> all;
			//사이즈만큼 늘려주기
			int ObjectSize = parent->m_pDynamicObject.size() + parent->m_pStaticObject.size();
			int PibotObject = 0;
			//2개이상일때 
			if (ObjectSize > 2) {
				all.resize(ObjectSize);
				//동적 오브젝트 합치기
				all.insert(std::next(parent->m_pDynamicObject.begin(), parent->m_pDynamicObject.size()), parent->m_pDynamicObject.begin(), parent->m_pDynamicObject.end());
				//정적 오브젝트 합치기
				all.insert(std::next(parent->m_pStaticObject.begin(), parent->m_pStaticObject.size()), parent->m_pStaticObject.begin(), parent->m_pStaticObject.end());
				//x로 정렬
				sort(all.begin(), all.end(), [](GameObject* a, GameObject* b) {return a->m_xmf4x4World._43 < b->m_xmf4x4World._43; });

				if (ObjectSize % 2 == 0) {
					//짝수
					PibotObject = ObjectSize + 1;

				}
				else {
					PibotObject = ObjectSize;
				}


				parent->m_pNodeUp->m_pStaticObject.clear();
				parent->m_pNodeDown->m_pStaticObject.clear();
				//동적 오브젝트 분류 
				for (const auto& mem : parent->m_pDynamicObject) {
					//오른쪽
					if (mem->m_xmf4x4World._43 > all[PibotObject]->m_xmf4x4World._43) {
						parent->m_pNodeUp->m_pDynamicObject.push_back(mem);
					}
					//왼쪽
					else if (mem->m_xmf4x4World._43 < all[PibotObject]->m_xmf4x4World._43) {
						parent->m_pNodeDown->m_pDynamicObject.push_back(mem);
					}

				}

				//정적 오브젝트 분류 
				for (const auto& mem : parent->m_pStaticObject) {


					//오른쪽
					if (mem->m_xmf4x4World._43 > all[PibotObject]->m_xmf4x4World._43) {
						parent->m_pNodeUp->m_pStaticObject.push_back(mem);
					}
					//왼쪽
					else if (mem->m_xmf4x4World._43 < all[PibotObject]->m_xmf4x4World._43) {
						parent->m_pNodeDown->m_pStaticObject.push_back(mem);
					}

				}
				UpdateNode(parent->m_pNodeUp, depth + 1);
				UpdateNode(parent->m_pNodeDown, depth + 1);

			}
			else {
				//예외 케이스.
			}


			
		}
	}

		
}