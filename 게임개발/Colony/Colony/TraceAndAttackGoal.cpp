#include "TraceAndAttackGoal.h"

void TraceAndAttack::Activate()
{
	m_iStatus = active;
	AddSubgoal(new TraceGoal(m_pOwner));

}

int TraceAndAttack::Process()
{
	PushGoalType();
	ActivateIfInactive();
	m_iStatus = ProcessSubGoals();

	if (m_pOwner->m_pPlayer) {
		XMFLOAT3 AlienPos = m_pOwner->GetPosition();
		XMFLOAT3 PlayerPos = m_pOwner->m_pPlayer->GetPosition();

		float dis = XM3CalDis(AlienPos, PlayerPos);
		if (m_pOwner->m_pPlayer) {

			float Dis = XM3CalDis(m_pOwner->m_pPlayer->m_xmf3FinalPosition, m_pOwner->GetPosition());
			if (Dis < 2.0f * m_pOwner->m_MonsterScale && m_SubGoals.front()->GetGoalType() != Attack_Goal) {
				AddSubgoal(new AttackGoal(m_pOwner, 1.0f));
			}
		}
		if (m_SubGoals.size() < 3) {
			if (m_SubGoals.front()->GetGoalType() != Jump_Goal && m_pOwner->m_HP != 100) {
				AddSubgoal(new JumpGoal(m_pOwner, 1.0f));
			}
		}
	}



	return m_iStatus;
}




void TraceAndAttack::Terminate()
{
	RemoveAllSubgoals();
}


