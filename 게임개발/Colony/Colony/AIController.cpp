#include "AIController.h"

AIController::AIController(GoalThink* pBrain, AlienSpider* pBody) :m_pBrain(pBrain), m_pBody(pBody){

};

bool AIController::ExecuteGoal(float fTimeElapsed)
{
	m_pBrain->Process();
	

	if (m_pBody->m_GoalType == FollowPath_Goal || m_pBody->m_GoalType == Wander_Goal) {
		//�ִϸ��̼�
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Run_2)) {
			m_pAnimationControl->ChangeAnimation(Run_2);
			m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 2.0f);
			m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 2.0f);
		}

		//����
		ExecuteFollowPath(fTimeElapsed);
	}
	else if (m_pBody->m_GoalType == Wait_Goal) {

		if ((!m_pAnimationControl->isSameState(AlienAnimationName::Idle_1)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_2)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_3)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_4)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Range_1)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Range_2))) {
			m_pAnimationControl->ChangeAnimation(Idle_1 + IdleRandom(gen));

			m_pBody->m_xmf3Velocity.x = 0.f;
			m_pBody->m_xmf3Velocity.z = 0.f;
		}

		ExecuteWait(fTimeElapsed);
	}
	else if (m_pBody->m_GoalType == Trace_Goal) {
		//�ִϸ��̼�
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Run_2)) {
			m_pAnimationControl->ChangeAnimation(Run_2);
			m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 2.0f);
			m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 2.0f);
		}
		MoveDest(fTimeElapsed);
	}
	else if (m_pBody->m_GoalType == Hitted_Goal) {
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Hit)) {
			m_pAnimationControl->ChangeAnimation(Hit);
			m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 3.0f);
			m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 3.0f);
		}
		m_pBody->m_xmf3Velocity.x = 0.f;
		m_pBody->m_xmf3Velocity.z = 0.f;
		m_pBody->m_WaitingTime += fTimeElapsed;
	}
	else if (m_pBody->m_GoalType == Deaded_Goal) {
		if ((!m_pAnimationControl->isSameState(AlienAnimationName::Death_1)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Death_2)) &&
			(!m_pAnimationControl->isSameState(AlienAnimationName::Death_3))) {
				
			m_pAnimationControl->ChangeAnimation(Death_1 + IdleRandom(gen)%3);
			m_pBody->m_xmf3Velocity.x = 0.f;
			m_pBody->m_xmf3Velocity.z = 0.f;


			if (m_pAnimationControl->isSameState(AlienAnimationName::Death_3)) {
				m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 1.0f);
				m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 1.0f);
			}
			else if (m_pAnimationControl->isSameState(AlienAnimationName::Death_2))
			{
				m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 0.6f);
				m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 0.6f);
			}
			else {
				m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 0.5f);
				m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 0.5f);
			}
		}
		m_pBody->m_WaitingTime += fTimeElapsed;
	}
	else if (m_pBody->m_GoalType == Attack_Goal) {
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Attack_8)
			) {
			m_pAnimationControl->ChangeAnimation(Attack_8);
			m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 1.0f);
			m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 1.0f);
		}
		m_pBody->m_WaitingTime += fTimeElapsed;

	}
	else if (m_pBody->m_GoalType == Jump_Goal) {

		//����
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Jump) && m_JumpStep == JUMP_PREPARE) {
			m_pAnimationControl->ChangeAnimation(AlienAnimationName::Jump);
			OutputDebugStringA("��������\n");
			m_JumpStep = JUMPING;
			m_pAnimationControl->SetTrackSpeed(NOW_TRACK, 2.0f);
			m_pAnimationControl->SetTrackSpeed(PRE_TRACK, 2.0f);

		}
		//�ϴ� ������
		else if (!m_pAnimationControl->isSameState(AlienAnimationName::Run_1) && m_pAnimationControl->isAnimationPlayProgress(Jump, 0.1f)
			&& m_JumpStep == JUMPING) {
			m_pAnimationControl->ChangeAnimation(AlienAnimationName::Run_1);

	

		}
		//����
		else if (!m_pAnimationControl->isSameState(AlienAnimationName::Jump) && m_JumpStep == JUMP_LANDING) {
			m_pAnimationControl->ChangeAnimation(AlienAnimationName::Jump);
			m_pAnimationControl->SetAnimationPlayPos(Jump, 0.6f);


		}
		else if (m_pAnimationControl->isAnimationPlayProgress(AlienAnimationName::Jump, 0.7f) && m_JumpStep == JUMP_LANDING) {
			m_JumpStep = JUMP_END;

		}

		if(m_JumpStep < JUMP_LANDING) JumpDest(fTimeElapsed);

	}


	return true;
}

void AIController::ExecuteFollowPath(float fTimeElapsed)
{
	if (m_pBody) {

		m_pBody->m_StuckTime += fTimeElapsed; 
		m_pBody->m_WaitCoolTime += fTimeElapsed;

		XMFLOAT2 ObjectPos{ m_pBody->GetPosition().x , m_pBody->GetPosition().z };
		float Distance = XM2CalDis(m_dest, ObjectPos);

		//��ü�� �������� �����ߴ��� üũ 
		if ((Distance < 1.6f || !m_StartTravel) && m_pBody->m_Path.size() > 0) {
			if (!m_StartTravel) {
				m_dest = m_pBody->m_Path.front();
				m_pBody->m_Path.pop_front();
				m_StartTravel = true;
			}
			if (m_pBody->m_Path.size() > 0) {
				m_dest = m_pBody->m_Path.front();
				m_pBody->m_Path.pop_front();
			}
			m_pBody->m_StuckTime = 0.f;

		}



		MoveDest(fTimeElapsed);
	}
		
		


}

void AIController::ExecuteWait(float fTimeElapsed)
{
	m_pBody->m_WaitingTime += fTimeElapsed;
}

void AIController::ExecuteWanderPath(float fTimeElapsed)
{


}

void AIController::MoveDest(float fTimeElapsed){
	
	
	// ��ü�� �̵� �� ȸ��
	XMFLOAT2 ObjectPos{ m_pBody->GetPosition().x , m_pBody->GetPosition().z };
	float Distance = XM2CalDis(m_dest, ObjectPos);

	XMFLOAT3 XM3ObjectPos = m_pBody->GetPosition();
	XMFLOAT3 XM3DestPos{ m_dest.x,0,m_dest.y };

	//�������� �����ϴ� ����
	m_dir = Vector3::Subtract(XM3DestPos, XM3ObjectPos);
	m_dir = Vector3::Normalize(m_dir);


	//ȸ��
	XMFLOAT3 ObjectDir = XMFLOAT3(m_pBody->GetLook());


	float dotProduct = Vector3::DotProduct(ObjectDir, m_dir);//��ü�� ����� �̵������� ����..
	float v1Size = VectorSize(ObjectDir);//���̰��� ���ϱ� ���� ���͵��� ũ�ⱸ�ϱ�
	float v2Size = VectorSize(m_dir);
	m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//���̰� ���ϱ�
	m_CrossBetween = Vector3::Normalize(Vector3::CrossProduct(ObjectDir, m_dir));//�ִ� ȸ���� ���ϱ� ���� ����
	float angle;
	//���� ����ó��
	if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
	else angle = 0;
	if (std::isnan(angle)) {
		angle = 0.f;
	}



	if (angle > 10.0f) {
		float Rotation = AlienSpinSpeed * fTimeElapsed;
		if (abs(Rotation) > abs(angle))  Rotation = angle;

		if (Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), m_CrossBetween) > 0)
			m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(Rotation), 0.0f), m_pBody->m_xmf4x4ToParent);
		else
			m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(-Rotation), 0.0f), m_pBody->m_xmf4x4ToParent);
	}

	if (angle < 30) {
		m_pBody->m_xmf3Velocity.x = 0;
		m_pBody->m_xmf3Velocity.z = 0;
		//�̵�
		Distance = XM2CalDis(m_dest, ObjectPos);
		m_pBody->m_xmf3Velocity.x += Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false).x;
		m_pBody->m_xmf3Velocity.z += Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false).z;
		//�ӷ� = �ð� * ���ӵ�
		//m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false));
	}
	


}

void AIController::JumpDest(float fTimeElapsed)
{

	// ��ü�� �̵� �� ȸ��
	XMFLOAT2 ObjectPos;
	float Distance ;

	XMFLOAT3 XM3ObjectPos;
	XMFLOAT3 XM3DestPos;
	if (m_pBody->m_pPlayer) {
		 ObjectPos = XMFLOAT2{ m_pBody->GetPosition().x , m_pBody->GetPosition().z };
		 Distance = XM2CalDis(m_dest, ObjectPos);

		 XM3ObjectPos = m_pBody->GetPosition();
		 XM3DestPos= XMFLOAT3{ m_dest.x,0,m_dest.y };

		//�������� �����ϴ� ����
		m_dir = Vector3::Subtract(XM3DestPos, XM3ObjectPos);
		m_dir = Vector3::Normalize(m_dir);
	}
	else {
	
		ObjectPos = XMFLOAT2{ m_pBody->GetPosition().x , m_pBody->GetPosition().z };
		Distance = XM2CalDis(m_dest, ObjectPos);

		XM3ObjectPos = m_pBody->GetPosition();
		XM3DestPos = XMFLOAT3{ m_dest.x,0,m_dest.y };
		XM3ObjectPos.y = 0;
		//�������� �����ϴ� ����
		m_dir = Vector3::Subtract(XM3DestPos, XM3ObjectPos);
		m_dir = Vector3::Normalize(m_dir);
	}

	//ȸ��
	XMFLOAT3 ObjectDir = XMFLOAT3(m_pBody->GetLook());


	float dotProduct = Vector3::DotProduct(ObjectDir, m_dir);//��ü�� ����� �̵������� ����..
	float v1Size = VectorSize(ObjectDir);//���̰��� ���ϱ� ���� ���͵��� ũ�ⱸ�ϱ�
	float v2Size = VectorSize(m_dir);
	m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//���̰� ���ϱ�
	m_CrossBetween = Vector3::Normalize(Vector3::CrossProduct(ObjectDir, m_dir));//�ִ� ȸ���� ���ϱ� ���� ����
	float angle;
	//���� ����ó��
	if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
	else angle = 0;
	if (std::isnan(angle)) {
		angle = 0.f;
	}

	//if (Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), m_CrossBetween) > 0)
	//	m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), AlienSpinSpeed * fTimeElapsed, 0.0f), m_pBody->m_xmf4x4ToParent);
	//else
	//	m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), -AlienSpinSpeed * fTimeElapsed, 0.0f), m_pBody->m_xmf4x4ToParent);


	////�߷�
	//m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	////�̵�
	//Distance = XM2CalDis(m_dest, ObjectPos);
	////�ӷ� = �ð� * ���ӵ�
	//m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_dir, fTimeElapsed * AlienJumpAccel, false));

	if (angle > 10.0f) {
		float Rotation = AlienSpinSpeed * fTimeElapsed;
		if (abs(Rotation) > abs(angle))  Rotation = angle;

		if (Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), m_CrossBetween) > 0)
			m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(Rotation), 0.0f), m_pBody->m_xmf4x4ToParent);
		else
			m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(-Rotation), 0.0f), m_pBody->m_xmf4x4ToParent);
	}

		//m_pBody->m_xmf3Velocity.x = 0;
		//m_pBody->m_xmf3Velocity.z = 0;
		//�̵�
		Distance = XM2CalDis(m_dest, ObjectPos);

		//�ӷ� = �ð� * ���ӵ�
		
		//m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false));
		m_pBody->m_xmf3Velocity.x += Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false).x;
		m_pBody->m_xmf3Velocity.z += Vector3::ScalarProduct(m_dir, fTimeElapsed * 1000.f, false).z;




}