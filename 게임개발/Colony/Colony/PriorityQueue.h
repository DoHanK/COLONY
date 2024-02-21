#pragma once
#include "stdafx.h"


class IndexPriorityQueue{
public:
	IndexPriorityQueue(std::vector<double>& costs, int NodeCount) :m_iMaxSize(NodeCount), m_iSize(0), m_vecCost(costs) {
		m_Heap.assign(NodeCount + 1, 0);
		m_invHeap.assign(NodeCount + 1, 0);
	};

	bool empty()const { return (m_iSize == 0); }

	void insert(const int idx) {
		assert(m_iSize + 1 <= m_iMaxSize);
		
		++m_iSize;

		m_Heap[m_iSize] = idx;

		m_invHeap[idx] = m_iSize;

		ReorderUpwards(m_iSize);
	}

	int Pop() {

		Swap(1, m_iSize);

		ReorderDownwards(1, m_iSize - 1);

		return m_Heap[m_iSize--];
	}

	void ChangePriority(const int idx) {
		ReorderUpwards(m_invHeap[idx]);
	}
private:
	std::vector<double>& m_vecCost;

	std::vector<int>  m_Heap; //노드 값

	std::vector<int>  m_invHeap; //m_Heap에 저장되어있는 노드 위치

	int				  m_iSize;
	int				  m_iMaxSize;
private:
	void Swap(int a, int b) {
		int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

		m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
	}

	void ReorderUpwards(int nd) {

		while ((nd > 1) && (m_vecCost[m_Heap[nd / 2]] > m_vecCost[m_Heap[nd]])) {

			Swap(nd / 2, nd);
			nd /= 2;
		}
	}

	void ReorderDownwards(int nd, int HeapSize) {

		while (2 * nd <= HeapSize) {
			int child = 2 * nd;
			
			if ((child < HeapSize) && (m_vecCost[m_Heap[child]] > m_vecCost[m_Heap[child + 1]])) {
				++child;
			}

			if (m_vecCost[m_Heap[nd]] > m_vecCost[m_Heap[child]]) {
				Swap(child, nd);

				nd = child;
			}

			else
			{
				break;
			}
		}
	}

};

