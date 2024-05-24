#include "TraceGoal.h"

TraceGoal::TraceGoal(AlienSpider* pOwner):Goal<AlienSpider>(pOwner, Trace_Goal)
{
}

TraceGoal::~TraceGoal()
{
}

void TraceGoal::Activate()
{
	m_iStatus = active;


}

int TraceGoal::Process()
{

	ActivateIfInactive();

	PushGoalType();

	if (m_pOwner->m_pPlayer) {//Player�߰�

		XMFLOAT3 PlayerPos = m_pOwner->m_pPlayer->GetPosition();

		m_pOwner->m_pSoul->m_dest = XMFLOAT2(PlayerPos.x, PlayerPos.z);


	}
	else { // �÷��̾ ���� ����.
		m_iStatus = failed;
	}

	return m_iStatus;
}

void TraceGoal::Terminate()
{
}
