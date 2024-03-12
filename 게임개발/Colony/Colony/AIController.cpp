#include "AIController.h"

AIController::AIController(GoalThink* pBrain, AlienSpider* pBody) :m_pBrain(pBrain), m_pBody(pBody){

};


bool AIController::ExecuteGoal(float fTimeElapsed)
{
	m_pBrain->Process();

	//길찾기
	if (m_pBody->m_GoalType == FollowPath_Goal) {
		//애니메이션
		if (!m_pAnimationControl->isSameState(AlienAnimationName::Run_2)) {
			m_pAnimationControl->ChangeAnimation(Run_2);
		}
	
		//연산
		ExecuteFollowPath(fTimeElapsed);
	}
	//멍때리기
	else if (m_pBody->m_GoalType == Wait_Goal) {

		
			if ((!m_pAnimationControl->isSameState(AlienAnimationName::Idle_1))&&
				(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_2))&&
				(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_3))&&
				(!m_pAnimationControl->isSameState(AlienAnimationName::Idle_4))) {

				m_pAnimationControl->ChangeAnimation(Idle_1 + IdleRandom(gen));
			}





		ExecuteWait(fTimeElapsed);

	}




	return true;
}

void AIController::ExecuteFollowPath(float fTimeElapsed)
{
	if (m_pBody) {
	
		m_pBody->m_WaitCoolTime += fTimeElapsed;

			XMFLOAT2 ObjectPos{ m_pBody->GetPosition().x , m_pBody->GetPosition().z };
			float Distance = XM2CalDis(m_dest, ObjectPos);

			//객체가 목적지에 도착했는지 체크 
			if ((Distance < 1.6f || !m_StartTravel )&& m_pBody->m_Path.size() > 0) {
				if (!m_StartTravel) {
					m_dest = m_pBody->m_Path.front();
					m_pBody->m_Path.pop_front();
					m_StartTravel = true;
				}
				if (m_pBody->m_Path.size() > 0) {
					m_dest = m_pBody->m_Path.front();
					m_pBody->m_Path.pop_front();
				}
				XMFLOAT3 XM3ObjectPos = m_pBody->GetPosition();
				XMFLOAT3 XM3DestPos{ m_dest.x,0,m_dest.y };
				
				//목적지로 가야하는 방향
				m_dir = Vector3::Subtract(XM3DestPos, XM3ObjectPos);
				m_dir = Vector3::Normalize(m_dir);
		
			}
			// 객체의 이동 및 회전
			

				XMFLOAT3 XM3ObjectPos = m_pBody->GetPosition();
				XMFLOAT3 XM3DestPos{ m_dest.x,0,m_dest.y };

				//목적지로 가야하는 방향
				m_dir = Vector3::Subtract(XM3DestPos, XM3ObjectPos);
				m_dir = Vector3::Normalize(m_dir);


				//회전
				XMFLOAT3 ObjectDir = XMFLOAT3(m_pBody->GetLook());
			
			
				float dotProduct = Vector3::DotProduct(ObjectDir, m_dir);//물체의 방향과 이동방향을 내적..
				float v1Size = VectorSize(ObjectDir);//사이각을 구하기 위한 벡터들의 크기구하기
				float v2Size = VectorSize(m_dir);
				m_BetweenAngle = acosf(dotProduct / (v1Size * v2Size));//사이각 구하기
				m_CrossBetween = Vector3::Normalize(Vector3::CrossProduct(ObjectDir, m_dir));//최단 회전을 구하기 위한 외적
				float angle;
				//각도 예외처리
				if (std::acosf(-1) != 0)  angle = m_BetweenAngle * (180.f / std::acosf(-1));
				else angle = 0;
				if (std::isnan(angle)) {
					angle = 0.f;
				}
		


				if (angle > 10.0f) {

					if (Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), m_CrossBetween) > 0)
						m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), AlienSpinSpeed * fTimeElapsed, 0.0f), m_pBody->m_xmf4x4ToParent);
					else
						m_pBody->m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), -AlienSpinSpeed * fTimeElapsed, 0.0f), m_pBody->m_xmf4x4ToParent);
				}
				if(angle < 90.0f){


					//이동
					Distance = XM2CalDis(m_dest, ObjectPos);
					//속력 = 시간 * 가속도
					m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_dir, fTimeElapsed * AlienAccel, false));

					//최대 속력을 넘었을때
					if (Vector3::Length(m_pBody->m_xmf3Velocity) > AlienMaxXZSpeed) {
						m_pBody->m_xmf3Velocity.x *= (AlienMaxXZSpeed / Vector3::Length(m_pBody->m_xmf3Velocity));
						m_pBody->m_xmf3Velocity.z *= (AlienMaxXZSpeed / Vector3::Length(m_pBody->m_xmf3Velocity));
					}




					XMFLOAT3 pos = Vector3::ScalarProduct(m_pBody->m_xmf3Velocity, fTimeElapsed, false);


					//	//거리보다 속력이 가까울때
					//if (Vector3::Length(pos) > Distance) {
					//	pos.x*= (Distance / Vector3::Length(pos));
					//	pos.z*= (Distance / Vector3::Length(pos));
					//}

					m_pBody->AddPostion(pos);


					//마찰력
					float fLength = Vector3::Length(m_pBody->m_xmf3Velocity);
					float fDeceleration = (friction * fTimeElapsed);
					if (fDeceleration > fLength) fDeceleration = fLength;
					m_pBody->m_xmf3Velocity = Vector3::Add(m_pBody->m_xmf3Velocity, Vector3::ScalarProduct(m_pBody->m_xmf3Velocity, -fDeceleration, true));

				}
	
			
		}
		
		


}

void AIController::ExecuteWait(float fTimeElapsed)
{
	m_pBody->m_WaitingTime += fTimeElapsed;
}
